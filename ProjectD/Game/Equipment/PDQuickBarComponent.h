// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"
#include "Game/Inventory/PDInventoryItemInstance.h"

#include "PDQuickBarComponent.generated.h"

class AActor;
class UPDEquipmentInstance;
class UPDEquipmentManagerComponent;
class UObject;
struct FFrame;


/** 
   어떤 무기를 장착할지에대한 관리를 해준다. 
   대신 무기를 한개씩 장착하는 형태로 관리 되고 있다.
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UPDQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UPDQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="PD")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category="PD")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="PD")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	TArray<UPDInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return _ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UPDInventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, UPDInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UPDInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	virtual void BeginPlay() override;

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();

	UPDEquipmentManagerComponent* FindEquipmentManager() const;

protected:
	UPROPERTY()
	int32 NumSlots = 3;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<UPDInventoryItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 _ActiveSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<UPDEquipmentInstance> _EquippedItem;
};


USTRUCT(BlueprintType)
struct FPDQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<UPDInventoryItemInstance>> Slots;
};


USTRUCT(BlueprintType)
struct FPDQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 ActiveIndex = 0;
};
