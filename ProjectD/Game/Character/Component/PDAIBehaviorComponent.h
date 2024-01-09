// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PDAIBehaviorComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTD_API UPDAIBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()
	

public:
	static FName TargetToFollow;
	static FName DistToTarget;
	static FName MoveToLocation;
	static FName isRanged;
};
