// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Game/Inventory/PDInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UPDEquipmentDefinition;
class UObject;

UCLASS()
class UInventoryFragment_EquippableItem : public UPDInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=PD)
	TSubclassOf<UPDEquipmentDefinition> EquipmentDefinition;
};
