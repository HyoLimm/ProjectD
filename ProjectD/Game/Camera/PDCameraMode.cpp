// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDCameraMode.h"

#include "Components/CapsuleComponent.h"
#include "Engine/Canvas.h"
#include "GameFramework/Character.h"
#include "Game/Character/Component/PDCameraComponent.h"
#include "PDPlayerCamManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDCameraMode)


//////////////////////////////////////////////////////////////////////////
// FPDCameraModeView
//////////////////////////////////////////////////////////////////////////
FPDCameraModeView::FPDCameraModeView()
	: _Location(ForceInit)
	, _Rotation(ForceInit)
	, _ControlRotation(ForceInit)
	, _FieldOfView(PD_CAMERA_DEFAULT_FOV)
{
}

void FPDCameraModeView::Blend(const FPDCameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	else if (OtherWeight >= 1.0f)
	{
		*this = Other;
		return;
	}

	_Location = FMath::Lerp(_Location, Other._Location, OtherWeight);

	const FRotator DeltaRotation = (Other._Rotation - _Rotation).GetNormalized();
	_Rotation = _Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other._ControlRotation - _ControlRotation).GetNormalized();
	_ControlRotation = _ControlRotation + (OtherWeight * DeltaControlRotation);

	_FieldOfView = FMath::Lerp(_FieldOfView, Other._FieldOfView, OtherWeight);
}


//////////////////////////////////////////////////////////////////////////
// UPDCameraMode
//////////////////////////////////////////////////////////////////////////
UPDCameraMode::UPDCameraMode()
{
	_FieldOfView = PD_CAMERA_DEFAULT_FOV;
	_ViewPitchMin = PD_CAMERA_DEFAULT_PITCH_MIN;
	_ViewPitchMax = PD_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.5f;
	BlendFunction = EPDCameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
}

UPDCameraComponent* UPDCameraMode::GetPDCameraComponent() const
{
	return CastChecked<UPDCameraComponent>(GetOuter());
}

UWorld* UPDCameraMode::GetWorld() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? nullptr : GetOuter()->GetWorld();
}

AActor* UPDCameraMode::GetTargetActor() const
{
	const UPDCameraComponent* PDCameraComponent = GetPDCameraComponent();

	return PDCameraComponent->GetTargetActor();
}

FVector UPDCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// Height adjustments for characters to account for crouching.
		if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetPawn))
		{
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
			check(TargetCharacterCDO);

			const UCapsuleComponent* CapsuleComp = TargetCharacter->GetCapsuleComponent();
			check(CapsuleComp);

			const UCapsuleComponent* CapsuleCompCDO = TargetCharacterCDO->GetCapsuleComponent();
			check(CapsuleCompCDO);

			const float DefaultHalfHeight = CapsuleCompCDO->GetUnscaledCapsuleHalfHeight();
			const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
			const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + TargetCharacterCDO->BaseEyeHeight;

			return TargetCharacter->GetActorLocation() + (FVector::UpVector * HeightAdjustment);
		}

		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UPDCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

void UPDCameraMode::UpdateCameraMode(float DeltaTime)
{
	UpdateView(DeltaTime);
	UpdateBlending(DeltaTime);
}

void UPDCameraMode::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, _ViewPitchMin, _ViewPitchMax);

	_CurrentView._Location = PivotLocation;
	_CurrentView._Rotation = PivotRotation;
	_CurrentView._ControlRotation = _CurrentView._Rotation;
	_CurrentView._FieldOfView = _FieldOfView;
}

void UPDCameraMode::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);

	// Since we're setting the blend weight directly, we need to calculate the blend alpha to account for the blend function.
	const float InvExponent = (BlendExponent > 0.0f) ? (1.0f / BlendExponent) : 1.0f;

	switch (BlendFunction)
	{
	case EPDCameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;

	case EPDCameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case EPDCameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case EPDCameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	default:
		checkf(false, TEXT("SetBlendWeight: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

void UPDCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.0f)
	{
		BlendAlpha += (DeltaTime / BlendTime);
		BlendAlpha = FMath::Min(BlendAlpha, 1.0f);
	}
	else
	{
		BlendAlpha = 1.0f;
	}

	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;

	switch (BlendFunction)
	{
	case EPDCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;

	case EPDCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	case EPDCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	case EPDCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	default:
		checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

void UPDCameraMode::DrawDebug(UCanvas* Canvas) const
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetDrawColor(FColor::White);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("      PDCameraMode: %s (%f)"), *GetName(), BlendWeight));
}


//////////////////////////////////////////////////////////////////////////
// UPDCameraModeStack
//////////////////////////////////////////////////////////////////////////
UPDCameraModeStack::UPDCameraModeStack()
{
	_bIsActive = true;
}

void UPDCameraModeStack::ActivateStack()
{
	if (!_bIsActive)
	{
		_bIsActive = true;

		// Notify camera modes that they are being activated.
		for (UPDCameraMode* CameraMode : _CameraModeStack)
		{
			check(CameraMode);
			CameraMode->OnActivation();
		}
	}
}

void UPDCameraModeStack::DeactivateStack()
{
	if (_bIsActive)
	{
		_bIsActive = false;

		// Notify camera modes that they are being deactivated.
		for (UPDCameraMode* CameraMode : _CameraModeStack)
		{
			check(CameraMode);
			CameraMode->OnDeactivation();
		}
	}
}

void UPDCameraModeStack::PushCameraMode(TSubclassOf<UPDCameraMode> CameraModeClass)
{
	//PDCameraComponent에서 대부분 호출됌
	if (!CameraModeClass)
	{
		return;
	}

	if (UPDCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass))
	{
		int32 StackSize = _CameraModeStack.Num();

		if ((StackSize > 0) && (_CameraModeStack[0] == CameraMode))
		{
			// 들어온 파라미터가 이미 최상단이라면 return
			return;
		}

		// See if it's already in the stack and remove it.
		// Figure out how much it was contributing to the stack.
		int32 ExistingStackIndex = INDEX_NONE;
		float ExistingStackContribution = 1.0f;

		for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
		{
			if (_CameraModeStack[StackIndex] == CameraMode)
			{
				ExistingStackIndex = StackIndex;
				ExistingStackContribution *= CameraMode->GetBlendWeight();
				break;
			}
			else
			{
				ExistingStackContribution *= (1.0f - _CameraModeStack[StackIndex]->GetBlendWeight());
			}
		}

		if (ExistingStackIndex != INDEX_NONE)
		{
			_CameraModeStack.RemoveAt(ExistingStackIndex);
			StackSize--;
		}
		else
		{
			ExistingStackContribution = 0.0f;
		}

		// Decide what initial weight to start with.
		// 어떤 초기 가중치로 시작할지 결정합니다.
		const bool bShouldBlend = ((CameraMode->GetBlendTime() > 0.0f) && (StackSize > 0));
		const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);

		CameraMode->SetBlendWeight(BlendWeight);

		// Add new entry to top of stack.
		_CameraModeStack.Insert(CameraMode, 0);

		// Make sure stack bottom is always weighted 100%.
		_CameraModeStack.Last()->SetBlendWeight(1.0f);

		// Let the camera mode know if it's being added to the stack.
		if (ExistingStackIndex == INDEX_NONE)
		{
			CameraMode->OnActivation();
		}
	}
	else
	{
		check(CameraMode);
	}
}

bool UPDCameraModeStack::EvaluateStack(float DeltaTime, FPDCameraModeView& OutCameraModeView)
{
	if (!_bIsActive)
	{
		return false;
	}

	UpdateStack(DeltaTime);
	BlendStack(OutCameraModeView);

	return true;
}

UPDCameraMode* UPDCameraModeStack::GetCameraModeInstance(TSubclassOf<UPDCameraMode> CameraModeClass)
{
	check(CameraModeClass);
	for (UPDCameraMode* CameraMode : _CameraModeInstances)
	{
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// Not found, so we need to create it.
	UPDCameraMode* NewCameraMode = NewObject<UPDCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	_CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}

void UPDCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = _CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;

	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UPDCameraMode* CameraMode = _CameraModeStack[StackIndex];
		check(CameraMode);

		CameraMode->UpdateCameraMode(DeltaTime);

		if (CameraMode->GetBlendWeight() >= 1.0f)
		{
			// Everything below this mode is now irrelevant and can be removed.
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		// Let the camera modes know they being removed from the stack.
		for (int32 StackIndex = RemoveIndex; StackIndex < StackSize; ++StackIndex)
		{
			UPDCameraMode* CameraMode = _CameraModeStack[StackIndex];
			check(CameraMode);

			CameraMode->OnDeactivation();
		}

		_CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UPDCameraModeStack::BlendStack(FPDCameraModeView& OutCameraModeView) const
{
	const int32 StackSize = _CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// Start at the bottom and blend up the stack
	const UPDCameraMode* CameraMode = _CameraModeStack[StackSize - 1];
	check(CameraMode);

	OutCameraModeView = CameraMode->GetCameraModeView();

	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		CameraMode = _CameraModeStack[StackIndex];
		check(CameraMode);

		OutCameraModeView.Blend(CameraMode->GetCameraModeView(), CameraMode->GetBlendWeight());
	}
}

void UPDCameraModeStack::DrawDebug(UCanvas* Canvas) const
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetDrawColor(FColor::Green);
	DisplayDebugManager.DrawString(FString(TEXT("   --- Camera Modes (Begin) ---")));

	for (const UPDCameraMode* CameraMode : _CameraModeStack)
	{
		check(CameraMode);
		CameraMode->DrawDebug(Canvas);
	}

	DisplayDebugManager.SetDrawColor(FColor::Green);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   --- Camera Modes (End) ---")));
}

void UPDCameraModeStack::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const
{
	if (_CameraModeStack.Num() == 0)
	{
		OutWeightOfTopLayer = 1.0f;
		OutTagOfTopLayer = FGameplayTag();
		return;
	}
	else
	{
		UPDCameraMode* TopEntry = _CameraModeStack.Last();
		check(TopEntry);
		OutWeightOfTopLayer = TopEntry->GetBlendWeight();
		OutTagOfTopLayer = TopEntry->GetCameraTypeTag();
	}
}

