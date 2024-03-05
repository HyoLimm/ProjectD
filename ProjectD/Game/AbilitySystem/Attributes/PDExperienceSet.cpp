// Copyright(c) Depsee. All Rights Reserved.


#include "Game/AbilitySystem/Attributes/PDExperienceSet.h"
#include "GameplayEffectExtension.h"

#include "Game/AbilitySystem/Attributes/PDAttributeSet.h"
#include "Game/PDGameplayTags.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"

#include "GameFramework/GameplayMessageSubsystem.h"

UPDExperienceSet::UPDExperienceSet()
	: Experience(10.0f)
	, MaxExperience(100.0f)
{

}

bool UPDExperienceSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	return true;
}

void UPDExperienceSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UPDExperienceSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UPDExperienceSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);


	if (Data.EvaluatedData.Attribute == GetExperienceAttribute())
	{
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingExperienceAttribute())
	{
		//const float LocalIncomingXP = GetIncomingExperienceAttribute();
		SetIncomingExperience(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetMaxExperienceAttribute())
	{

	}
}

void UPDExperienceSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}
