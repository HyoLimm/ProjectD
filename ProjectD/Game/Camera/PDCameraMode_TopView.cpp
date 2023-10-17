// Fill out your copyright notice in the Description page of Project Settings.


#include "PDCameraMode_TopView.h"

UPDCameraMode_TopView::UPDCameraMode_TopView()
{
	
}


void  UPDCameraMode_TopView::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	//const double CameraLoftDistance = BoundsSizeToDistance.GetRichCurveConst()->Eval(100.0);	

	//PivotLocation = GetPivotLocation() +CameraLoftDistance;
	PivotLocation += Offset;

	_CurrentView.Location = PivotLocation;
	_CurrentView.Rotation = DefaultPivotRotation;
	_CurrentView.ControlRotation = _CurrentView.Rotation;
	_CurrentView._FieldOfView = _FieldOfView;
}