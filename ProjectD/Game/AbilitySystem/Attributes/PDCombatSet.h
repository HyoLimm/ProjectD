// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "PDAttributeSet.h"

#include "PDCombatSet.generated.h"

class UObject;
struct FFrame;


/**
 * UPDCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS(BlueprintType)
class UPDCombatSet : public UPDAttributeSet
{
	GENERATED_BODY()
public:
	UPDCombatSet();

	ATTRIBUTE_ACCESSORS(UPDCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UPDCombatSet, BaseHeal);

protected:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);
private:
	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "PD|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;



	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "PD|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;

};
