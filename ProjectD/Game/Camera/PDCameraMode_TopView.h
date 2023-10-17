// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Camera/PDCameraMode.h"
#include "PDCameraMode_TopView.generated.h"


UCLASS(Abstract, Blueprintable)
class PROJECTD_API UPDCameraMode_TopView : public UPDCameraMode
{
	GENERATED_BODY()
public:
	UPDCameraMode_TopView();
protected:
	//~UPDCameraMode interface
	virtual void UpdateView(float DeltaTime) override;
	//~End of UPDCameraMode interface

protected:

	UPROPERTY(EditDefaultsOnly, Category = "TopView")
	FVector  Offset;



	UPROPERTY(EditDefaultsOnly, Category = "TopView")
	FRotator DefaultPivotRotation;


	

	//UPROPERTY(EditDefaultsOnly, Category = "TopView")
	//FRuntimeFloatCurve BoundsSizeToDistance;
};
