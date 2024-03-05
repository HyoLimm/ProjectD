// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "EditCondition/WhenPlatformHasTrait.h"
#include "Game/Settings/CustomSettings/PDSettingValueDiscrete_Language.h"
#include "PDSettingsLocal.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "Game/Player/PDLocalPlayer.h"


#define LOCTEXT_NAMESPACE "PD"

UGameSettingCollection* UPDGameSettingRegistry::InitializeGameplaySettings(UPDLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("GameplayCollection"));
	Screen->SetDisplayName(LOCTEXT("GameplayCollection_Name", "Gameplay"));
	Screen->Initialize(InLocalPlayer);

	{
		UGameSettingCollection* LanguageSubsection = NewObject<UGameSettingCollection>();
		LanguageSubsection->SetDevName(TEXT("LanguageCollection"));
		LanguageSubsection->SetDisplayName(LOCTEXT("LanguageCollection_Name", "Language"));
		Screen->AddSetting(LanguageSubsection);

		//----------------------------------------------------------------------------------
		{
			UPDSettingValueDiscrete_Language* Setting = NewObject<UPDSettingValueDiscrete_Language>();
			Setting->SetDevName(TEXT("Language"));
			Setting->SetDisplayName(LOCTEXT("LanguageSetting_Name", "Language"));
			Setting->SetDescriptionRichText(LOCTEXT("LanguageSetting_Description", "The language of the game."));
			
#if WITH_EDITOR
			if (GIsEditor)
			{
				Setting->SetDescriptionRichText(LOCTEXT("LanguageSetting_WithEditor_Description", "The language of the game.\n\n<text color=\"#ffff00\">WARNING: Language changes will not affect PIE, you'll need to run with -game to test this, or change your PIE language options in the editor preferences.</>"));
			}
#endif
			
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			LanguageSubsection->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
	}

	//{
	//	UGameSettingCollection* ReplaySubsection = NewObject<UGameSettingCollection>();
	//	ReplaySubsection->SetDevName(TEXT("ReplayCollection"));
	//	ReplaySubsection->SetDisplayName(LOCTEXT("ReplayCollection_Name", "Replays"));
	//	Screen->AddSetting(ReplaySubsection);

	//	//----------------------------------------------------------------------------------
	//	{
	//		UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
	//		Setting->SetDevName(TEXT("RecordReplay"));
	//		Setting->SetDisplayName(LOCTEXT("RecordReplaySetting_Name", "Record Replays"));
	//		Setting->SetDescriptionRichText(LOCTEXT("RecordReplaySetting_Description", "Automatically record game replays. Experimental feature, recorded demos may have playback issues."));

	//		Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(ShouldAutoRecordReplays));
	//		Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetShouldAutoRecordReplays));
	//		Setting->SetDefaultValue(GetDefault<UPDSettingsLocal>()->ShouldAutoRecordReplays());

	//		Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
	//		Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(UPDReplaySubsystem::GetPlatformSupportTraitTag(), TEXT("Platform does not support saving replays")));

	//		ReplaySubsection->AddSetting(Setting);

	//	}
	//	//----------------------------------------------------------------------------------

	//	//----------------------------------------------------------------------------------
	//	{
	//		UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
	//		Setting->SetDevName(TEXT("KeepReplayLimit"));
	//		Setting->SetDisplayName(LOCTEXT("KeepReplayLimitSetting_Name", "Keep Replay Limit"));
	//		Setting->SetDescriptionRichText(LOCTEXT("KeepReplayLimitSetting_Description", "Number of saved replays to keep, set to 0 for infinite."));

	//		Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetNumberOfReplaysToKeep));
	//		Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetNumberOfReplaysToKeep));
	//		Setting->SetDefaultValue(GetDefault<UPDSettingsLocal>()->GetNumberOfReplaysToKeep());
	//		for (int32 Index = 0; Index <= 20; Index++)
	//		{
	//			Setting->AddOption(Index, FText::AsNumber(Index));
	//		}

	//		Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
	//		Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(UPDReplaySubsystem::GetPlatformSupportTraitTag(), TEXT("Platform does not support saving replays")));

	//		ReplaySubsection->AddSetting(Setting);

	//	}
	//	//----------------------------------------------------------------------------------
	//}

	return Screen;
}

#undef LOCTEXT_NAMESPACE
