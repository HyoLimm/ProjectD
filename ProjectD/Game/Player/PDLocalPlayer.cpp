// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Player/PDLocalPlayer.h"
#include "Game/Settings/PDSettingsLocal.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(PDLocalPlayer)

UPDLocalPlayer::UPDLocalPlayer()
{

}

void UPDLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();
}


void UPDLocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);
}


bool UPDLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	OnPlayerControllerChanged(PlayerController);

	return bResult;
}

UPDSettingsLocal* UPDLocalPlayer::GetLocalSettings() const
{
	return UPDSettingsLocal::Get();
}

UPDSettingsShared* UPDLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		SharedSettings = UPDSettingsShared::LoadOrCreateSettings(this);
	}

	return SharedSettings;
}

void UPDLocalPlayer::OnPlayerControllerChanged(APlayerController* NewController)
{

}

