// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDGameplayEffectContext.h"

#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Game/AbilitySystem/PDAbilitySourceInterface.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif



#include UE_INLINE_GENERATED_CPP_BY_NAME(PDGameplayEffectContext)

class FArchive;

FPDGameplayEffectContext* FPDGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if (BaseEffectContext != nullptr) 
	{
		if (BaseEffectContext->GetScriptStruct()->IsChildOf(FPDGameplayEffectContext::StaticStruct()))
		{
			return (FPDGameplayEffectContext*)BaseEffectContext;	
		}
	}

	return nullptr;
}

bool FPDGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

#if UE_WITH_IRIS
namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FPDGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
	//UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(PDGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif

void FPDGameplayEffectContext::SetAbilitySource(const IPDAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IPDAbilitySourceInterface* FPDGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IPDAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FPDGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

