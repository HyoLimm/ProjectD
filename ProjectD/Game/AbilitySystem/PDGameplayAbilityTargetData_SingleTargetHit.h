// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "PDGameplayAbilityTargetData_SingleTargetHit.generated.h"

class FArchive;
struct FGameplayEffectContextHandle;


/** Game-specific additions to SingleTargetHit tracking */
USTRUCT()
struct FPDGameplayAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()

	FPDGameplayAbilityTargetData_SingleTargetHit()
		: CartridgeID(-1)
	{ }

	virtual void AddTargetDataToContext(FGameplayEffectContextHandle& Context, bool bIncludeActorArray) const override;

	/** ID to allow the identification of multiple bullets that were part of the same cartridge */
	UPROPERTY()
	int32 CartridgeID;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FPDGameplayAbilityTargetData_SingleTargetHit::StaticStruct();
	}
};

template<>
struct TStructOpsTypeTraits<FPDGameplayAbilityTargetData_SingleTargetHit> : public TStructOpsTypeTraitsBase2<FPDGameplayAbilityTargetData_SingleTargetHit>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

