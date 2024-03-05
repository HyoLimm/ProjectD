// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Input/PDInputModifier.h"

#include "Game/Player/PDLocalPlayer.h"
#include "Game/Settings/PDSettingsShared.h"

//////////////////////////////////////////////////////////////////////
// PDInputModifiersHelpers

namespace PDInputModifiersHelpers
{
	/** Returns the owning PDLocalPlayer of an Enhanced Player Input pointer */
	static UPDLocalPlayer* GetLocalPlayer(const UEnhancedPlayerInput* PlayerInput)
	{
		if (PlayerInput)
		{
			if (APlayerController* PC = Cast<APlayerController>(PlayerInput->GetOuter()))
			{
				return Cast<UPDLocalPlayer>(PC->GetLocalPlayer());
			}
		}
		return nullptr;
	}

}

//////////////////////////////////////////////////////////////////////
// UPDSettingBasedScalar

FInputActionValue UPDSettingBasedScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	if (ensureMsgf(CurrentValue.GetValueType() != EInputActionValueType::Boolean, TEXT("Setting Based Scalar modifier doesn't support boolean values.")))
	{
		if (UPDLocalPlayer* LocalPlayer = PDInputModifiersHelpers::GetLocalPlayer(PlayerInput))
		{
			const UClass* SettingsClass = UPDSettingsShared::StaticClass();
			UPDSettingsShared* SharedSettings = LocalPlayer->GetSharedSettings();

			const bool bHasCachedProperty = PropertyCache.Num() == 3;

			const FProperty* XAxisValue = bHasCachedProperty ? PropertyCache[0] : SettingsClass->FindPropertyByName(XAxisScalarSettingName);
			const FProperty* YAxisValue = bHasCachedProperty ? PropertyCache[1] : SettingsClass->FindPropertyByName(YAxisScalarSettingName);
			const FProperty* ZAxisValue = bHasCachedProperty ? PropertyCache[2] : SettingsClass->FindPropertyByName(ZAxisScalarSettingName);

			if (PropertyCache.IsEmpty())
			{
				PropertyCache.Emplace(XAxisValue);
				PropertyCache.Emplace(YAxisValue);
				PropertyCache.Emplace(ZAxisValue);
			}

			FVector ScalarToUse = FVector(1.0, 1.0, 1.0);

			switch (CurrentValue.GetValueType())
			{
			case EInputActionValueType::Axis3D:
				ScalarToUse.Z = ZAxisValue ? *ZAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				//[[fallthrough]];
			case EInputActionValueType::Axis2D:
				ScalarToUse.Y = YAxisValue ? *YAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				//[[fallthrough]];
			case EInputActionValueType::Axis1D:
				ScalarToUse.X = XAxisValue ? *XAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				break;
			}

			ScalarToUse.X = FMath::Clamp(ScalarToUse.X, MinValueClamp.X, MaxValueClamp.X);
			ScalarToUse.Y = FMath::Clamp(ScalarToUse.Y, MinValueClamp.Y, MaxValueClamp.Y);
			ScalarToUse.Z = FMath::Clamp(ScalarToUse.Z, MinValueClamp.Z, MaxValueClamp.Z);

			return CurrentValue.Get<FVector>() * ScalarToUse;
		}
	}

	return CurrentValue;
}

FInputActionValue UPDInputModifierAimInversion::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	UPDLocalPlayer* LocalPlayer = PDInputModifiersHelpers::GetLocalPlayer(PlayerInput);
	if (!LocalPlayer)
	{
		return CurrentValue;
	}

	UPDSettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	FVector NewValue = CurrentValue.Get<FVector>();

	if (Settings->GetInvertVerticalAxis())
	{
		NewValue.Y *= -1.0f;
	}

	if (Settings->GetInvertHorizontalAxis())
	{
		NewValue.X *= -1.0f;
	}

	return NewValue;
}
