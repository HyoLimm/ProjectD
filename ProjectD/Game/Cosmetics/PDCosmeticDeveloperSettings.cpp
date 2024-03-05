// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDCosmeticDeveloperSettings.h"
#include "Game/Cosmetics/PDCharacterPartTypes.h"
#include "Game/System/PDDevelopmentStatics.h"

#include "Misc/App.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "PDControllerComponent_CharacterParts.h"
#include "EngineUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDCosmeticDeveloperSettings)

#define LOCTEXT_NAMESPACE "PDCheats"

UPDCosmeticDeveloperSettings::UPDCosmeticDeveloperSettings()
{
}

FName UPDCosmeticDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

#if WITH_EDITOR

void UPDCosmeticDeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ApplySettings();
}

void UPDCosmeticDeveloperSettings::PostReloadConfig(FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);

	ApplySettings();
}

void UPDCosmeticDeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();

	ApplySettings();
}

void UPDCosmeticDeveloperSettings::ApplySettings()
{
	if (GIsEditor && (GEngine != nullptr))
	{
		ReapplyLoadoutIfInPIE();
	}
}

void UPDCosmeticDeveloperSettings::ReapplyLoadoutIfInPIE()
{
#if WITH_SERVER_CODE
	// Update the loadout on all players
	UWorld* ServerWorld = UPDDevelopmentStatics::FindPlayInEditorAuthorityWorld();
	if (ServerWorld != nullptr)
	{
		ServerWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([=]()
			{
				for (TActorIterator<APlayerController> PCIterator(ServerWorld); PCIterator; ++PCIterator)
				{
					if (APlayerController* PC = *PCIterator)
					{
						if (UPDControllerComponent_CharacterParts* CosmeticComponent = PC->FindComponentByClass<UPDControllerComponent_CharacterParts>())
						{
							CosmeticComponent->ApplyDeveloperSettings();
						}
					}
				}
			}));
	}
#endif	// WITH_SERVER_CODE
}

void UPDCosmeticDeveloperSettings::OnPlayInEditorStarted() const
{
	// Show a notification toast to remind the user that there's an experience override set
	if (CheatCosmeticCharacterParts.Num() > 0)
	{
		FNotificationInfo Info(LOCTEXT("CosmeticOverrideActive", "Applying Cosmetic Override"));
		Info.ExpireDuration = 2.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE

