// Copyright(c) Depsee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingRegistry.h"
#include "PDGameSettingRegistry.generated.h"

class UPDLocalPlayer;
class UGameSettingCollection;


DECLARE_LOG_CATEGORY_EXTERN(LogPDGameSettingRegistry, Log, Log);

#define GET_SHARED_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UPDLocalPlayer, GetSharedSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(UPDSettingsShared, FunctionOrPropertyName)		\
	}))

#define GET_LOCAL_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UPDLocalPlayer, GetLocalSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(UPDSettingsLocal, FunctionOrPropertyName)		\
	}))


UCLASS()
class PROJECTD_API UPDGameSettingRegistry : public UGameSettingRegistry
{
	GENERATED_BODY()
public:
	UPDGameSettingRegistry();

	static UPDGameSettingRegistry* Get(UPDLocalPlayer* InLocalPlayer);

	virtual void SaveChanges() override;

protected:
	virtual void OnInitialize(ULocalPlayer* InLocalPlayer) override;
	virtual bool IsFinishedInitializing() const override;

	/*UGameSettingCollection* InitializeVideoSettings(UPDLocalPlayer* InLocalPlayer);
	void InitializeVideoSettings_FrameRates(UGameSettingCollection* Screen, UPDLocalPlayer* InLocalPlayer);
	void AddPerformanceStatPage(UGameSettingCollection* Screen, UPDLocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeAudioSettings(UPDLocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGameplaySettings(UPDLocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeMouseAndKeyboardSettings(UPDLocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGamepadSettings(UPDLocalPlayer* InLocalPlayer);*/

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> VideoSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> AudioSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GameplaySettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> MouseAndKeyboardSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GamepadSettings;
};
