// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Camera/PDCameraMode.h"
#include "PDCameraMode_Turn.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class PROJECTD_API UPDCameraMode_Turn : public UPDCameraMode
{
	GENERATED_BODY()
protected:

	virtual void UpdateView(float DeltaTime) override;
};
