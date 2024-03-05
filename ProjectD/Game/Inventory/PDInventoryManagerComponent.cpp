// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "PDInventoryItemDefinition.h"
#include "PDInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_PD_Inventory_Message_StackChanged, "PD.Inventory.Message.StackChanged");

//////////////////////////////////////////////////////////////////////
// FPDInventoryEntry

FString FPDInventoryEntry::GetDebugString() const
{
	TSubclassOf<UPDInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FPDInventoryList

void FPDInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FPDInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FPDInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FPDInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FPDInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FPDInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FPDInventoryList::BroadcastChangeMessage(FPDInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FPDInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_PD_Inventory_Message_StackChanged, Message);
}

UPDInventoryItemInstance* FPDInventoryList::AddEntry(TSubclassOf<UPDInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UPDInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FPDInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UPDInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDef);
	for (UPDInventoryItemFragment* Fragment : GetDefault<UPDInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//const UPDInventoryItemDefinition* ItemCDO = GetDefault<UPDInventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);

	return Result;
}

void FPDInventoryList::AddEntry(UPDInventoryItemInstance* Instance)
{
	unimplemented();
}

void FPDInventoryList::RemoveEntry(UPDInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FPDInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

TArray<UPDInventoryItemInstance*> FPDInventoryList::GetAllItems() const
{
	TArray<UPDInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FPDInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

//////////////////////////////////////////////////////////////////////
// UPDInventoryManagerComponent

UPDInventoryManagerComponent::UPDInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UPDInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UPDInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

UPDInventoryItemInstance* UPDInventoryManagerComponent::AddItemDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UPDInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UPDInventoryManagerComponent::AddItemInstance(UPDInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UPDInventoryManagerComponent::RemoveItemInstance(UPDInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UPDInventoryItemInstance*> UPDInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UPDInventoryItemInstance* UPDInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef) const
{
	for (const FPDInventoryEntry& Entry : InventoryList.Entries)
	{
		UPDInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UPDInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FPDInventoryEntry& Entry : InventoryList.Entries)
	{
		UPDInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UPDInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UPDInventoryItemInstance* Instance = UPDInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

void UPDInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UPDInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FPDInventoryEntry& Entry : InventoryList.Entries)
		{
			UPDInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UPDInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FPDInventoryEntry& Entry : InventoryList.Entries)
	{
		UPDInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class UPDInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(UPDInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class UPDInventoryFilter_HasTag : public UPDInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(UPDInventoryItemInstance* Instance) const { return true; }
// };


