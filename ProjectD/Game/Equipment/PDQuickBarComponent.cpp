// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDQuickBarComponent.h"

#include "Game/Equipment/PDEquipmentDefinition.h"
#include "Game/Equipment/PDEquipmentInstance.h"
#include "Game/Equipment/PDEquipmentManagerComponent.h"
#include "Game/Inventory/InventoryFragment_EquippableItem.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDQuickBarComponent)

class FLifetimeProperty;
class UPDEquipmentDefinition;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_PD_QuickBar_Message_SlotsChanged, "PD.QuickBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_PD_QuickBar_Message_ActiveIndexChanged, "PD.QuickBar.Message.ActiveIndexChanged");

UPDQuickBarComponent::UPDQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UPDQuickBarComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, _ActiveSlotIndex);
}

void UPDQuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

void UPDQuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (_ActiveSlotIndex < 0 ? Slots.Num()-1 : _ActiveSlotIndex);
	int32 NewIndex = _ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UPDQuickBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (_ActiveSlotIndex < 0 ? Slots.Num()-1 : _ActiveSlotIndex);
	int32 NewIndex = _ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UPDQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(_ActiveSlotIndex));
	check(_EquippedItem == nullptr);

	if (UPDInventoryItemInstance* SlotItem = Slots[_ActiveSlotIndex])
	{
		//장착가능한 UInventoryFragment_EquippableItem로 지정되어 있는것만 가져오기
		if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
		{			
			if (TSubclassOf<UPDEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition)
			{
				if (UPDEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					_EquippedItem = EquipmentManager->EquipItem(EquipDef);
					if (_EquippedItem != nullptr)
					{
						_EquippedItem->SetInstigator(SlotItem);
					}
				}
			}
		}
	}
}

void UPDQuickBarComponent::UnequipItemInSlot()
{
	if (UPDEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (_EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(_EquippedItem);
			_EquippedItem = nullptr;
		}
	}
}

UPDEquipmentManagerComponent* UPDQuickBarComponent::FindEquipmentManager() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = OwnerController->GetPawn())
		{
			return UPDEquipmentManagerComponent::FindEquipmentManagerComponent(Pawn);			
		}
	}
	return nullptr;
}

void UPDQuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (_ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot();

		_ActiveSlotIndex = NewIndex;

		EquipItemInSlot();

		OnRep_ActiveSlotIndex();
	}
}

UPDInventoryItemInstance* UPDQuickBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(_ActiveSlotIndex) ? Slots[_ActiveSlotIndex] : nullptr;
}

int32 UPDQuickBarComponent::GetNextFreeItemSlot() const
{
	int32 SlotIndex = 0;
	for (TObjectPtr<UPDInventoryItemInstance> ItemPtr : Slots)
	{
		if (ItemPtr == nullptr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

void UPDQuickBarComponent::AddItemToSlot(int32 SlotIndex, UPDInventoryItemInstance* Item)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
			OnRep_Slots();
		}
	}
}

UPDInventoryItemInstance* UPDQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	UPDInventoryItemInstance* Result = nullptr;

	if (_ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		_ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
			OnRep_Slots();
		}
	}

	return Result;
}

void UPDQuickBarComponent::OnRep_Slots()
{
	FPDQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_PD_QuickBar_Message_SlotsChanged, Message);
}

void UPDQuickBarComponent::OnRep_ActiveSlotIndex()
{
	FPDQuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = _ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_PD_QuickBar_Message_ActiveIndexChanged, Message);
}

