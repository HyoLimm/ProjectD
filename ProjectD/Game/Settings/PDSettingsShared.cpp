// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDSettingsShared.h"
#include "Kismet/GameplayStatics.h"
#include "Game/Player/PDLocalPlayer.h"

static FString SHARED_SETTINGS_SLOT_NAME = TEXT("SharedGameSettings");

void UPDSettingsShared::Initialize(UPDLocalPlayer* LocalPlayer)
{
	check(LocalPlayer);

	OwningPlayer = LocalPlayer;
}

UPDSettingsShared* UPDSettingsShared::LoadOrCreateSettings(const UPDLocalPlayer* LocalPlayer)
{
	UPDSettingsShared* SharedSettings = nullptr;

	// 세이브 게임이 존재하는 경우 로딩.
	if (UGameplayStatics::DoesSaveGameExist(SHARED_SETTINGS_SLOT_NAME, LocalPlayer->GetLocalPlayerIndex()))
	{
		USaveGame* Slot = UGameplayStatics::LoadGameFromSlot(SHARED_SETTINGS_SLOT_NAME, LocalPlayer->GetLocalPlayerIndex());
		SharedSettings = Cast<UPDSettingsShared>(Slot);
	}

	if (SharedSettings == nullptr)
	{
		SharedSettings = Cast<UPDSettingsShared>(UGameplayStatics::CreateSaveGameObject(UPDSettingsShared::StaticClass()));
	}

	SharedSettings->Initialize(const_cast<UPDLocalPlayer*>(LocalPlayer));
	SharedSettings->ApplySettings();

	return SharedSettings;
}

void UPDSettingsShared::ApplySettings()
{

}
