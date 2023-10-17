// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/SaveGame.h"
//#include "SubtitleDisplayOptions.h"

#include "UObject/ObjectPtr.h"
#include "PDSettingsShared.generated.h"

class UObject;
struct FFrame;

UENUM(BlueprintType)
enum class EColorBlindMode : uint8
{
	Off,
	// Deuteranope (green weak/blind)
	Deuteranope,
	// Protanope (red weak/blind)
	Protanope,
	// Tritanope(blue weak / bind)
	Tritanope
};

UENUM(BlueprintType)
enum class EPDAllowBackgroundAudioSetting : uint8
{
	Off,
	AllSounds,

	Num UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EPDGamepadSensitivity : uint8
{
	Invalid = 0		UMETA(Hidden),

	Slow			UMETA(DisplayName = "01 - Slow"),
	SlowPlus		UMETA(DisplayName = "02 - Slow+"),
	SlowPlusPlus	UMETA(DisplayName = "03 - Slow++"),
	Normal			UMETA(DisplayName = "04 - Normal"),
	NormalPlus		UMETA(DisplayName = "05 - Normal+"),
	NormalPlusPlus	UMETA(DisplayName = "06 - Normal++"),
	Fast			UMETA(DisplayName = "07 - Fast"),
	FastPlus		UMETA(DisplayName = "08 - Fast+"),
	FastPlusPlus	UMETA(DisplayName = "09 - Fast++"),
	Insane			UMETA(DisplayName = "10 - Insane"),

	MAX				UMETA(Hidden),
};

class UPDLocalPlayer;

/**
 * ULyraSettingsShared - The "Shared" settings are stored as part of the USaveGame system, these settings are not machine
 * specific like the local settings, and are safe to store in the cloud - and 'share' them.  Using the save game system
 * we can also store settings per player, so things like controller keybind preferences should go here, because if those
 * are stored in the local settings all users would get them.
 *
 * TODO NDarnell Future version rename this maybe to CloudSave?  Even though these arent necessarily in the cloud...
 *               maybe change Localsettings to LyraPlatformSettings, or DeviceSettings?  Make this one UserSettings?  TBD
 *               
 * NOTE: I want to do Within=LocalPlayer, but SaveGames create the object in the transient package, instead
 * of getting to select the outer, maybe LoadGameFromMemory should have a variant, like LoadGameFromMemory_WithOuter, or maybe pass in
 * an optional outer.
 */
UCLASS(/*Within=LocalPlayer*/)
class UPDSettingsShared : public USaveGame
{
	GENERATED_BODY()

public:
	//DECLARE_EVENT_OneParam(UPDSettingsShared, FOnSettingChangedEvent, UPDSettingsShared* Settings);
	//FOnSettingChangedEvent OnSettingChanged;



public:
	//UPDSettingsShared();

	void Initialize(UPDLocalPlayer* LocalPlayer);
	static UPDSettingsShared* LoadOrCreateSettings(const UPDLocalPlayer* LocalPlayer);

	void ApplySettings();

	UFUNCTION()
	bool GetInvertVerticalAxis() const { return bInvertVerticalAxis; }

	UFUNCTION()
	bool GetInvertHorizontalAxis() const { return bInvertHorizontalAxis; }

private:
	/** If true then the vertical look axis should be inverted */
	UPROPERTY()
	bool bInvertVerticalAxis = true;

	/** If true then the horizontal look axis should be inverted */
	UPROPERTY()
	bool bInvertHorizontalAxis = false;


	/** Holds the mouse horizontal sensitivity */
	UPROPERTY()
	double MouseSensitivityX = 0.5;

	/** Holds the mouse vertical sensitivity */
	UPROPERTY()
	double MouseSensitivityY = 0.5;


private:
	UPROPERTY(Transient)
	TObjectPtr<UPDLocalPlayer> OwningPlayer = nullptr;
};
