// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDSettingsShared.h"

#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Internationalization/Culture.h"

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
	//ApplySubtitleOptions();
	//ApplyBackgroundAudioSettings();
	ApplyCultureSettings();

	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
	{
		if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
		{
			InputSettings->ApplySettings();
		}
	}
}

void UPDSettingsShared::ApplyCultureSettings()
{
	if (bResetToDefaultCulture)
	{
		const FCulturePtr SystemDefaultCulture = FInternationalization::Get().GetDefaultCulture();
		check(SystemDefaultCulture.IsValid());

		const FString CultureToApply = SystemDefaultCulture->GetName();
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			// Clear this string
			GConfig->RemoveKey(TEXT("Internationalization"), TEXT("Culture"), GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		bResetToDefaultCulture = false;
	}
	else if (!PendingCulture.IsEmpty())
	{
		// SetCurrentCulture may trigger PendingCulture to be cleared (if a culture change is broadcast) so we take a copy of it to work with
		const FString CultureToApply = PendingCulture;
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			// Note: This is intentionally saved to the users config
			// We need to localize text before the player logs in and very early in the loading screen
			GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *CultureToApply, GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		ClearPendingCulture();
	}
}

void UPDSettingsShared::ResetCultureToCurrentSettings()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

const FString& UPDSettingsShared::GetPendingCulture() const
{
	return PendingCulture;
}

void UPDSettingsShared::SetPendingCulture(const FString& NewCulture)
{
	PendingCulture = NewCulture;
	bResetToDefaultCulture = false;
	bIsDirty = true;
}

void UPDSettingsShared::OnCultureChanged()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

void UPDSettingsShared::ClearPendingCulture()
{
	PendingCulture.Reset();
}

bool UPDSettingsShared::IsUsingDefaultCulture() const
{
	FString Culture;
	GConfig->GetString(TEXT("Internationalization"), TEXT("Culture"), Culture, GGameUserSettingsIni);

	return Culture.IsEmpty();
}

void UPDSettingsShared::ResetToDefaultCulture()
{
	ClearPendingCulture();
	bResetToDefaultCulture = true;
	bIsDirty = true;
}

