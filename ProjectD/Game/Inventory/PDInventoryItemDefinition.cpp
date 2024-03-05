// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDInventoryItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDInventoryItemDefinition)

//////////////////////////////////////////////////////////////////////
// UPDInventoryItemDefinition

UPDInventoryItemDefinition::UPDInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UPDInventoryItemFragment* UPDInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UPDInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UPDInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// UPDInventoryItemDefinition

const UPDInventoryItemFragment* UPDInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UPDInventoryItemDefinition> ItemDef, TSubclassOf<UPDInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UPDInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

