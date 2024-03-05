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


	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:
	//~ Begin UActorComponent interface
	virtual void					OnRegister() override;
	virtual void					GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
	//~ End UActorComponent interface


	virtual void					UpdateCameraModes();

protected:

	// 카메라 모드를 블렌드하는 데 사용되는 스택.
	UPROPERTY()
		TObjectPtr<UPDCameraModeStack> _CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float _FieldOfViewOffset;
};
