// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PDActivatableWidget.h"

#include "PDHUDLayout.generated.h"

class UCommonActivatableWidget;
class UObject;


UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "PD HUD Layout", Category = "PD|HUD"))
class UPDHUDLayout : public UPDActivatableWidget
{
	GENERATED_BODY()

public:

	UPDHUDLayout(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
};
