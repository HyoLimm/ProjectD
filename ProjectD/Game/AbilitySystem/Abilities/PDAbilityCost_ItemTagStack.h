// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "PDAbilityCost.h"
#include "ScalableFloat.h"

#include "PDAbilityCost_ItemTagStack.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UPDGameplayAbility;
class UObject;
struct FGameplayAbilityActorInfo;

/**
 * Represents a cost that requires expending a quantity of a tag stack
 * on the associated item instance
 */
UCLASS(meta=(DisplayName="Item Tag Stack"))
class UPDAbilityCost_ItemTagStack : public UPDAbilityCost
{
	GENERATED_BODY()

public:
	UPDAbilityCost_ItemTagStack();

	//~UPDAbilityCost interface
	virtual bool CheckCost(const UPDGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UPDGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of UPDAbilityCost interface

protected:
	/** How much of the tag to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	FScalableFloat Quantity;

	/** Which tag to spend some of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	FGameplayTag Tag;

	/** Which tag to send back as a response if this cost cannot be applied */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	FGameplayTag FailureTag;
};
