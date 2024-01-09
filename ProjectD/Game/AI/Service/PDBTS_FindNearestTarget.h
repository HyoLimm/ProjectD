// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "PDBTS_FindNearestTarget.generated.h"

class AActor;
class AAIController;


UCLASS()
class PROJECTD_API UPDBTS_FindNearestTarget : public UBTService
{
	GENERATED_BODY()
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	void		FindNearestTarget(UBehaviorTreeComponent& OwnerComp);
protected:
	/** Cached AIController owner of BehaviorTreeComponent. */
	UPROPERTY(Transient)
	AAIController* mAIOwner;

	/** Cached actor owner of BehaviorTreeComponent. */
	UPROPERTY(Transient)
	AActor* mActorOwner;
};
