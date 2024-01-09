// Fill out your copyright notice in the Description page of Project Settings.


#include "PDCameraComponent.h"
#include "Game/Camera/PDCameraMode.h"


UPDCameraComponent::UPDCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	_CameraModeStack = nullptr;
	_FieldOfViewOffset = 0.0f;
}

void UPDCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (nullptr == _CameraModeStack)
	{
		_CameraModeStack = NewObject<UPDCameraModeStack>(this);
		check(_CameraModeStack);
	}
}

void UPDCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& OutDesiredView)
{
	check(_CameraModeStack);
	UpdateCameraModes();

	FPDCameraModeView CameraModeView;
	_CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);
	
	//�÷��̾� ��Ʈ�ѷ��� �� �����̼ǰ� ����ȭ
		if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
		{
			if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
			{
				PC->SetControlRotation(CameraModeView._ControlRotation);
			}
		}

	// Apply any offset that was added to the field of view.
	// �þ߿� �߰��� �������� �����մϴ�.
	CameraModeView._FieldOfView += _FieldOfViewOffset;
	//_FieldOfViewOffset = 0.0f;

	// Keep camera component in sync with the latest view.
	// camera component�� �ֽ� view�� ����ȭ�մϴ�.
	SetWorldLocationAndRotation(CameraModeView._Location, CameraModeView._Rotation);
	FieldOfView = CameraModeView._FieldOfView;

	// Fill in desired view.
	OutDesiredView.Location = CameraModeView._Location;
	OutDesiredView.Rotation = CameraModeView._Rotation;
	OutDesiredView.FOV = CameraModeView._FieldOfView;
	OutDesiredView.OrthoWidth = OrthoWidth;
	OutDesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	OutDesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	OutDesiredView.AspectRatio = AspectRatio;
	OutDesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	OutDesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	OutDesiredView.ProjectionMode = ProjectionMode;

	// See if the CameraActor wants to override the PostProcess settings used.
    // Camera Actor�� ���� Post Process ������ ���������� Ȯ���մϴ�.
	OutDesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
	{
		OutDesiredView.PostProcessSettings = PostProcessSettings;
	}
}

void UPDCameraComponent::UpdateCameraModes()
{
	check(_CameraModeStack);

	if (_CameraModeStack->IsStackActivate())
	{		
		if (DetermineCameraModeDelegate.IsBound())
		{
			//PDHeroComponent���� ī�޶� ��� ������
			if (const TSubclassOf<UPDCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
			{
				_CameraModeStack->PushCameraMode(CameraMode);
			}
		}
	}
}
