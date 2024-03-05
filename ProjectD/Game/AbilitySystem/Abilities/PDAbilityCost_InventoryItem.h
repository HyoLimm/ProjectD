// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PDAbilityCost.h"
#include "ScalableFloat.h"
#include "Templates/SubclassOf.h"

#include "PDAbilityCost_InventoryItem.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UPDGameplayAbility;
class UPDInventoryItemDefinition;
class UObject;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * Represents a cost that requires expending a quantity of an inventory item
 */
UCLASS(meta=(DisplayName="Inventory Item"))
class UPDAbilityCost_InventoryItem : public UPDAbilityCost
{
	GENERATED_BODY()

public:
	UPDAbilityCost_InventoryItem();

	//~UPDAbilityCost interface
	virtual bool CheckCost(const UPDGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UPDGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of UPDAbilityCost interface

protected:
	/** How much of the item to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	FScalableFloat Quantity;

	/** Which item to consume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	TSubclassOf<UPDInventoryItemDefinition> ItemDefinition;
};
