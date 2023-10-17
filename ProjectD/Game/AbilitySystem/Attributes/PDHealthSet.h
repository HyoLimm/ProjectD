// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "Game/AbilitySystem/Attributes/PDAttributeSet.h"
#include "PDHealthSet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTD_API UPDHealthSet : public UPDAttributeSet
{
	GENERATED_BODY()
public:

	UPDHealthSet();

	ATTRIBUTE_ACCESSORS(UPDHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UPDHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UPDHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UPDHealthSet, Damage);


protected:
	//// Delegate to broadcast when the health attribute reaches zero.
	//mutable FPDAttributeEvent OnOutOfHealth;
private:

	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly,  Category = "PD|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, Category = "PD|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;


	// Used to track when the health reaches 0.
	bool bOutOfHealth;

private:
	// Incoming healing. This is mapped directly to +Health
	UPROPERTY(BlueprintReadOnly, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category = "Lyra|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
};
