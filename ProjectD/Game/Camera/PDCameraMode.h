// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/World.h"
#include "GameplayTagContainer.h"

#include "PDCameraMode.generated.h"

class AActor;
class UCanvas;
class UPDCameraComponent;

/**
 * EPDCameraModeBlendFunction
 *
 *	Blend function used for transitioning between camera modes.
 */
UENUM(BlueprintType)
enum class EPDCameraModeBlendFunction : uint8
{
	// Does a simple linear interpolation.
	Linear,

	// Immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by the exponent.
	EaseIn,

	// Smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by the exponent.
	EaseOut,

	// Smoothly accelerates and decelerates.  Ease amount controlled by the exponent.
	EaseInOut,

	COUNT	UMETA(Hidden)
};


/**
 * FPDCameraModeView
 *
 *	View data produced by the camera mode that is used to blend camera modes.
 *  // PDCameraMode를 블렌드 하는 데 사용되는 카메라 모드에서 생성된 뷰 데이터.
 */
struct FPDCameraModeView
{
public:

	FPDCameraModeView();

	void Blend(const FPDCameraModeView& Other, float OtherWeight);

public:

	FVector  _Location;
	FRotator _Rotation;
	FRotator _ControlRotation;
	float    _FieldOfView;
};


/**
 * UPDCameraMode
 *
 * 프로젝트 PD의 카메라모드들의 베이스 클래스 역할
 */
UCLASS(Abstract, NotBlueprintable)
class PROJECTD_API UPDCameraMode : public UObject
{
	GENERATED_BODY()

public:

	UPDCameraMode();

	UPDCameraComponent* GetPDCameraComponent() const;

	virtual UWorld* GetWorld() const override;

	AActor* GetTargetActor() const;

	const FPDCameraModeView& GetCameraModeView() const { return _CurrentView; }

	// Called when this camera mode is activated on the camera mode stack.
	virtual void OnActivation() {};

	// Called when this camera mode is deactivated on the camera mode stack.
	virtual void OnDeactivation() {};

	void UpdateCameraMode(float DeltaTime);

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);

	FGameplayTag GetCameraTypeTag() const
	{
		return CameraTypeTag;
	}

	virtual void DrawDebug(UCanvas* Canvas) const;

protected:

	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;

	virtual void UpdateView(float DeltaTime);
	virtual void UpdateBlending(float DeltaTime);

protected:
	// A tag that can be queried by gameplay code that cares when a kind of camera mode is active
	// without having to ask about a specific mode (e.g., when aiming downsights to get more accuracy)
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	FGameplayTag CameraTypeTag;

	// View output produced by the camera mode.
	FPDCameraModeView _CurrentView;

	// The horizontal field of view (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float _FieldOfView;

	// Minimum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float _ViewPitchMin;

	// Maximum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float _ViewPitchMax;

	// How long it takes to blend in this mode.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	// Function used for blending.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	EPDCameraModeBlendFunction BlendFunction;

	// Exponent used by blend functions to control the shape of the curve.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	// Linear blend alpha used to determine the blend weight.
	// blend weight를 결정하는 데 사용되는 Linear blend alpha.
	float BlendAlpha;

	// Blend weight calculated using the blend alpha and function.
	// blend alpha 그리고 함수를 사용하여 계산된 Blend weight
	float BlendWeight;

protected:
	/** If true, skips all interpolation and puts camera in ideal location.  Automatically set to false next frame. */
	UPROPERTY(transient)
	uint32 bResetInterpolation:1;
};


/**
 * UPDCameraModeStack
 *
 *	Stack used for blending camera modes.
 */
UCLASS()
class UPDCameraModeStack : public UObject
{
	GENERATED_BODY()

public:

	UPDCameraModeStack();

	void ActivateStack();
	void DeactivateStack();

	bool IsStackActivate() const { return _bIsActive; }

	void PushCameraMode(TSubclassOf<UPDCameraMode> CameraModeClass);

	bool EvaluateStack(float DeltaTime, FPDCameraModeView& OutCameraModeView);

	void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	UPDCameraMode* GetCameraModeInstance(TSubclassOf<UPDCameraMode> CameraModeClass);

	void UpdateStack(float DeltaTime);
	void BlendStack(FPDCameraModeView& OutCameraModeView) const;

protected:

	bool _bIsActive;

	UPROPERTY()
	TArray<TObjectPtr<UPDCameraMode>> _CameraModeInstances;

	UPROPERTY()
	TArray<TObjectPtr<UPDCameraMode>> _CameraModeStack;
};
