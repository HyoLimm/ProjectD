// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDEquipmentManagerComponent.h"

#include "Game/AbilitySystem/PDAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "PDEquipmentDefinition.h"
#include "PDEquipmentInstance.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDEquipmentManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

//////////////////////////////////////////////////////////////////////
// FPDAppliedEquipmentEntry

FString FPDAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

//////////////////////////////////////////////////////////////////////
// FPDEquipmentList

void FPDEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FPDAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
		}
	}
}

void FPDEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FPDAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

void FPDEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// 	for (int32 Index : ChangedIndices)
	// 	{
	// 		const FGameplayTagStack& Stack = Stacks[Index];
	// 		TagToCountMap[Stack.Tag] = Stack.StackCount;
	// 	}
}

UPDAbilitySystemComponent* FPDEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UPDAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

UPDEquipmentInstance* FPDEquipmentList::AddEntry(TSubclassOf<UPDEquipmentDefinition> EquipmentDefinition)
{
	UPDEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UPDEquipmentDefinition* EquipmentCDO = GetDefault<UPDEquipmentDefinition>(EquipmentDefinition);
	check(EquipmentCDO);

	TSubclassOf<UPDEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UPDEquipmentInstance::StaticClass();
	}

	FPDAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UPDEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewEntry.Instance;


	if (UPDAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const UPDAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		check(false);
		//@TODO: Warning logging?
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);


	MarkItemDirty(NewEntry);

	return Result;
}

void FPDEquipmentList::RemoveEntry(UPDEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FPDAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UPDAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();


			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// UPDEquipmentManagerComponent

UPDEquipmentManagerComponent::UPDEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UPDEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

UPDEquipmentInstance* UPDEquipmentManagerComponent::EquipItem(TSubclassOf<UPDEquipmentDefinition> EquipmentClass)
{
	do
	{
		if (nullptr == EquipmentClass)
		{
			break;
		}

		UPDEquipmentInstance* Result = EquipmentList.AddEntry(EquipmentClass);
		if (nullptr == Result)
		{
			break;
		}

		Result->OnEquipped();

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(Result);
		}

		return Result;
	} while (false);

	check(false);
	return nullptr;
}

void UPDEquipmentManagerComponent::UnequipItem(UPDEquipmentInstance* ItemInstance)
{
	if (ItemInstance)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

bool UPDEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FPDAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UPDEquipmentInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UPDEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UPDEquipmentManagerComponent::UninitializeComponent()
{
	TArray<UPDEquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FPDAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UPDEquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

void UPDEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing PDEquipmentInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FPDAppliedEquipmentEntry& Entry : EquipmentList.Entries)
		{
			UPDEquipmentInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

UPDEquipmentInstance* UPDEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UPDEquipmentInstance> InstanceType)
{
	for (FPDAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UPDEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UPDEquipmentInstance*> UPDEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UPDEquipmentInstance> InstanceType) const
{
	TArray<UPDEquipmentInstance*> Results;
	for (const FPDAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UPDEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}


