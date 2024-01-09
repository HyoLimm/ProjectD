// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AI/Service/PDBTS_DistanceToTarget.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Game/Character/Component/PDAIBehaviorComponent.h"

void UPDBTS_DistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() && OwnerComp.GetAIOwner()->GetPawn() )
	{		
		auto ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
		check(ControlledPawn);
		
		if (AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetToFollow"))))
		{
			const float ResultDistance = ControlledPawn->GetDistanceTo(TargetActor);
			
			OwnerComp.GetBlackboardComponent()->SetValueAsFloat(UPDAIBehaviorComponent::DistToTarget, ResultDistance);
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(UPDAIBehaviorComponent::MoveToLocation, TargetActor->GetActorLocation());
		}
		
	}
}