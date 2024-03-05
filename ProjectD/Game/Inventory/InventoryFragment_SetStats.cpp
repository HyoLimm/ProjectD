// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryFragment_SetStats.h"

#include "Game/Inventory/PDInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_SetStats)

void UInventoryFragment_SetStats::OnInstanceCreated(UPDInventoryItemInstance* Instance) const
{
	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStatTagStack(KVP.Key, KVP.Value);
	}
}

int32 UInventoryFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}
