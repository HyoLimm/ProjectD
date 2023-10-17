// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "PDCameraComponent.generated.h"

class UPDCameraModeStack;
class UPDCameraMode;

DECLARE_DELEGATE_RetVal(TSubclassOf<UPDCameraMode>, FPDCameraModeDelegate);


UCLASS()
class PROJECTD_API UPDCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
public:

	UPDCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "PD|Camera")
	static UPDCameraComponent*		FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPDCameraComponent>() : nullptr); }

	// Returns the target actor that the camera is looking at.
	virtual AActor*					GetTargetActor() const { return GetOwner(); }

	FPDCameraModeDelegate			DetermineCameraModeDelegate;

protected:
	//~ Begin UActorComponent interface
	virtual void					OnRegister() override;
	virtual void					GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
	//~ End UActorComponent interface


	virtual void					UpdateCameraModes();

protected:

	// Stack used to blend the camera modes.
	UPROPERTY()
		TObjectPtr<UPDCameraModeStack> CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float FieldOfViewOffset;
};
