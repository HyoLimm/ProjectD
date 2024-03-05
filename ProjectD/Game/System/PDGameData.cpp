// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDGameData.h"
#include "PDAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDGameData)

UPDGameData::UPDGameData()
{
}

const UPDGameData& UPDGameData::UPDGameData::Get()
{
	return UPDAssetManager::Get().GetGameData();
}
