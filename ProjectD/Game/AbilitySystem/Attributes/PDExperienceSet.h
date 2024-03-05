// Copyright(c) Depsee. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "Game/AbilitySystem/Attributes/PDAttributeSet.h"
#include "PDExperienceSet.generated.h"


struct FGameplayEffectModCallbackData;

UCLASS(BlueprintType)
class UPDExperienceSet : public UPDAttributeSet
{
	GENERATED_BODY()
public:
	UPDExperienceSet();
	mutable FPDAttributeEvent OnExperienceChanged;
	mutable FPDAttributeEvent OnMaxExperienceChanged;
	


	ATTRIBUTE_ACCESSORS(UPDExperienceSet, Experience);
	ATTRIBUTE_ACCESSORS(UPDExperienceSet, MaxExperience);
	ATTRIBUTE_ACCESSORS(UPDExperienceSet, IncomingExperience);
protected:
	//~UAttributeSet interface
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	//~End of UAttributeSet interface

private:
	UPROPERTY(BlueprintReadOnly, Category = "PD|Exp", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Experience;


	UPROPERTY(BlueprintReadOnly, Category = "PD|Exp", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxExperience;


	UPROPERTY(BlueprintReadOnly, Category = "PD|Exp", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingExperience;
};
