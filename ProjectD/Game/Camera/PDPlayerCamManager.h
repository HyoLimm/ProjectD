// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PDPlayerCamManager.generated.h"

#define PD_CAMERA_DEFAULT_FOV			(80.0f)
#define PD_CAMERA_DEFAULT_PITCH_MIN		(-89.0f)
#define PD_CAMERA_DEFAULT_PITCH_MAX		(89.0f)


UCLASS()
class PROJECTD_API APDPlayerCamManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	//virtual void		UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
public:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	void SpawnPlayerCamera();
};
