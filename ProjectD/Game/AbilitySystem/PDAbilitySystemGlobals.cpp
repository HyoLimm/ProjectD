// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDAbilitySystemGlobals.h"

#include "PDGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDAbilitySystemGlobals)

struct FGameplayEffectContext;

UPDAbilitySystemGlobals::UPDAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UPDAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FPDGameplayEffectContext();
}

