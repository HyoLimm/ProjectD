// Fill out your copyright notice in the Description page of Project Settings.


#include "PDPawnData.h"

UPDPawnData::UPDPawnData(const FObjectInitializer& ObjectInitializer)	
	:Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
	DefaultCameraMode = nullptr;
}
