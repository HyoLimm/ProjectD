// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "PDSettingsLocal.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTD_API UPDSettingsLocal : public UGameUserSettings
{
	GENERATED_BODY()
public:


	UPDSettingsLocal();

	static UPDSettingsLocal* Get();



	const TMap<FName, FKey>& GetCustomPlayerInputConfig() const { return CustomKeyboardConfig; }

private:
	/** Array of custom key mappings that have been set by the player. Empty by default. */
	UPROPERTY(Config)
	TMap<FName, FKey> CustomKeyboardConfig;
};
