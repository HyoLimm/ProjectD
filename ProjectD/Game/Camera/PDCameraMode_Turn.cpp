// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Camera/PDCameraMode_Turn.h"

void UPDCameraMode_Turn::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation() /* + OffsetBlendPct*/;
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, _ViewPitchMin, _ViewPitchMax);

	_CurrentView._Location = PivotLocation;
	_CurrentView._Rotation = PivotRotation;
	_CurrentView._ControlRotation = _CurrentView._Rotation;
	_CurrentView._FieldOfView = _FieldOfView;
}
