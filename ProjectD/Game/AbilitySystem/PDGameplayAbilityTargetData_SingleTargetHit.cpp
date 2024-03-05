// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDGameplayAbilityTargetData_SingleTargetHit.h"

#include "PDGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDGameplayAbilityTargetData_SingleTargetHit)

struct FGameplayEffectContextHandle;

//////////////////////////////////////////////////////////////////////

void FPDGameplayAbilityTargetData_SingleTargetHit::AddTargetDataToContext(FGameplayEffectContextHandle& Context, bool bIncludeActorArray) const
{
	FGameplayAbilityTargetData_SingleTargetHit::AddTargetDataToContext(Context, bIncludeActorArray);

	// Add game-specific data
	if (FPDGameplayEffectContext* TypedContext = FPDGameplayEffectContext::ExtractEffectContext(Context))
	{
		TypedContext->CartridgeID = CartridgeID;
	}
}

bool FPDGameplayAbilityTargetData_SingleTargetHit::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayAbilityTargetData_SingleTargetHit::NetSerialize(Ar, Map, bOutSuccess);

	Ar << CartridgeID;

	return true;
}

