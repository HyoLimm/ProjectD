// Fill out your copyright notice in the Description page of Project Settings.


#include "PDPlayerCamManager.h"
#include "PDPlayerController.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "PDPlayerCamera.h"
#include "Engine/World.h"

//void APDPlayerCamManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
//{
//	// Don't update outgoing viewtarget during an interpolation 
//	if ((PendingViewTarget.Target != NULL) && BlendParams.bLockOutgoing && OutVT.Equal(ViewTarget))
//	{
//		return;
//	}
//
//
//	auto _PlayerController = Cast<APDPlayerController>(PCOwner);
//	auto _TargetPawn = GetViewTarget();
//
//	if (_PlayerController != nullptr)
//	{
//		// store previous POV, in case we need it later
//		FMinimalViewInfo OrigPOV = OutVT.POV;
//
//		//@TODO: CAMERA: Should probably reset the view target POV fully here
//		OutVT.POV.FOV = DefaultFOV;
//		OutVT.POV.OrthoWidth = DefaultOrthoWidth;
//		OutVT.POV.AspectRatio = DefaultAspectRatio;
//		OutVT.POV.bConstrainAspectRatio = bDefaultConstrainAspectRatio;
//		OutVT.POV.bUseFieldOfViewForLOD = true;
//		OutVT.POV.ProjectionMode = bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
//		OutVT.POV.PostProcessSettings.SetBaseValues();
//		OutVT.POV.PostProcessBlendWeight = 1.0f;
//
//		bool bDoNotApplyModifiers = false;
//
//		
//
//		FVector finalLocation;
//		FQuat finalRotation;
//		if (ACameraActor* CamActor = Cast<ACameraActor>(OutVT.Target))
//		{
//			// Viewing through a camera actor.
//			if (CamActor->GetCameraComponent() != nullptr)
//				CamActor->GetCameraComponent()->GetCameraView(DeltaTime, OutVT.POV);
//		}
//		else if (_TargetPawn != nullptr)
//		{
//			const FTransform pivotTarget = _TargetPawn->GetActorTransform();
//
//			//FRotator DesiredRot = _PlayerController->GetControlRotation();
//
//			//FRotator camRotationYaw = FRotator(0.0f, DesiredRot.Yaw, 0.0f);
//			//FVector tempCurVector = camRotationYaw.UnrotateVector(pivotTarget.GetLocation());
//			//PC_LOG_MSG_LOG(*FString::Format(TEXT("tempCurVector    Location : [{0}] "), { tempCurVector.ToString() }));
//			//ACameraActor* CamActor2 = Cast<ACameraActor>(OutVT.Target);
//
//			FVector ArmOrigin = pivotTarget.GetLocation();
//			FVector DesiredLoc = ArmOrigin;
//
//			// Now offset camera position back along our rotation
//			//DesiredLoc -= DesiredRot.Vector() * 500.0f;
//			// Add socket offset in local space
//			//DesiredLoc += FRotationMatrix(DesiredRot).TransformVector(FVector::ZeroVector);
//
//			FVector ResultLoc;
//			ResultLoc = DesiredLoc;
//
//
//			// Form a transform for new world transform for camera
//			//FTransform WorldCamTM(DesiredRot, ResultLoc);
//			//// Convert to relative to component
//			//FTransform RelCamTM = WorldCamTM.GetRelativeTransform(_TargetPawn->GetActorTransform());
//
//			// Update socket location/rotation
//			//_RelCamLocation = RelCamTM.GetLocation();
//			//_RelCamRotation = FMath::RInterpTo(OutVT.POV.Rotation, _PlayerController->GetControlRotation(), DeltaTime, 20.0f).Quaternion();
//			////FVector::Normalize()
//			////	ResultLoc.Normalize()
//
//			////_RelCamLocation += pivotTarget.GetLocation();
//			////_RelCamRotation = DesiredRot.Quaternion();
//			////m_TargetRota = FMath::RInterpTo(OutVT.POV.Rotation, _PlayerController->GetControlRotation(), DeltaTime, 20.0f);
//
//			//OutVT.POV.Location = _RelCamLocation;
//			////OutVT.POV.Rotation = _RelCamRotation.Rotator();
//			//OutVT.POV.Rotation = FLookAtMatrix(GetCameraLocation(), pivotTarget.GetLocation(), GetActorUpVector()).ToQuat().Rotator();
//
//			//PC_LOG_MSG_LOG(*FString::Format(TEXT("Cam    Location : [{0}] "), { _RelCamLocation.ToString() }));
//			//PC_LOG_MSG_LOG(*FString::Format(TEXT("Cam    Rot : [{0}] "), { _RelCamRotation.Rotator().ToString() }));
//			//FMath::
//
//
//			
//			finalLocation = _TargetPawn->GetActorTransform().GetLocation() + FVector(0.f, 0.f, 500);
//			//finalRotation = _RelCamRotation;
//		}
//
//
//		if (!bDoNotApplyModifiers || this->bAlwaysApplyModifiers)
//		{
//			// Apply camera modifiers at the end (view shakes for example)
//			ApplyCameraModifiers(DeltaTime, OutVT.POV);
//		}
//		// Synchronize the actor with the view target results
//		SetActorLocationAndRotation(finalLocation, finalRotation, false);
//		OutVT.POV.Location = finalLocation;
//		UpdateCameraLensEffects(OutVT);
//		Super::UpdateViewTarget(OutVT, DeltaTime);
//	}
//	else
//	{
//		//UE_LOG(PlayerCamManager, Warning, TEXT("Player Controller Not Valid"));
//		Super::UpdateViewTarget(OutVT, DeltaTime);
//	}
//}

void APDPlayerCamManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APDPlayerCamManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APDPlayerCamManager::SpawnPlayerCamera()
{
	//FTransform transform;
	//transform.SetLocation(FVector(0.0f,0.0f,0.f));
	//APDPlayerCamera* SpawnedCam = GetWorld()->SpawnActorDeferred<APDPlayerCamera>(APDPlayerCamera::StaticClass(), transform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	//if (SpawnedCam)
	//{
	//	//스폰하기전에 할일
	//	SpawnedCam->FinishSpawning(transform);
	//}	
}

void APDPlayerCamManager::BeginPlay()
{
	Super::BeginPlay();

	SpawnPlayerCamera();
}
