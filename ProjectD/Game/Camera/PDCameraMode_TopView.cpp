// Fill out your copyright notice in the Description page of Project Settings.


#include "PDCameraMode_TopView.h"

UPDCameraMode_TopView::UPDCameraMode_TopView()
{
	
}

void  UPDCameraMode_TopView::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();

	PivotLocation += _Offset;

	_CurrentView._Location = PivotLocation;
	_CurrentView._Rotation = _DefaultPivotRotation;
	_CurrentView._ControlRotation = _CurrentView._Rotation;
	_CurrentView._FieldOfView = _FieldOfView;
}