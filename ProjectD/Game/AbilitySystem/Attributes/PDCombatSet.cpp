// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDCombatSet.h"

#include "Game/AbilitySystem/Attributes/PDAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDCombatSet)

class FLifetimeProperty;


UPDCombatSet::UPDCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UPDCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPDCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPDCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UPDCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPDCombatSet, BaseDamage, OldValue);
}

void UPDCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPDCombatSet, BaseHeal, OldValue);
}

