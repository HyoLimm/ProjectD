// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDCosmeticAnimationTypes.h"

#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMesh.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDCosmeticAnimationTypes)

TSubclassOf<UAnimInstance> FPDAnimLayerSelectionSet::SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const
{
	for (const FPDAnimLayerSelectionEntry& Rule : LayerRules)
	{
		if ((Rule.Layer != nullptr) && CosmeticTags.HasAll(Rule.RequiredTags))
		{
			return Rule.Layer;
		}
	}

	return DefaultLayer;
}

USkeletalMesh* FPDAnimBodyStyleSelectionSet::SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const
{
	for (const FPDAnimBodyStyleSelectionEntry& Rule : MeshRules)
	{
		if ((Rule.Mesh != nullptr) && CosmeticTags.HasAll(Rule.RequiredTags))
		{
			return Rule.Mesh;
		}
	}

	return DefaultMesh;
}

