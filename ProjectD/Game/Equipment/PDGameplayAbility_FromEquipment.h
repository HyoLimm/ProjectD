// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Game/AbilitySystem/Abilities/PDGameplayAbility.h"

#include "PDGameplayAbility_FromEquipment.generated.h"

class UPDEquipmentInstance;
class UPDInventoryItemInstance;

/**
 * UPDGameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class UPDGameplayAbility_FromEquipment : public UPDGameplayAbility
{
	GENERATED_BODY()

public:

	UPDGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="PD|Ability")
	UPDEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "PD|Ability")
	UPDInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
