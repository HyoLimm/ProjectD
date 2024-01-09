// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AI/Service/PDBTS_FindNearestTarget.h"
#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Game/Character/Component/PDAIBehaviorComponent.h"

void UPDBTS_FindNearestTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (mAIOwner == nullptr && OwnerComp.GetAIOwner())
	{
		mAIOwner = OwnerComp.GetAIOwner();
	}
	if (mActorOwner == nullptr && OwnerComp.GetAIOwner()->GetPawn())
	{
		mActorOwner = OwnerComp.GetAIOwner()->GetPawn();
	}
	FindNearestTarget(OwnerComp);
}

void UPDBTS_FindNearestTarget::FindNearestTarget(UBehaviorTreeComponent& OwnerComp)
{
	float ClosestDistance = std::numeric_limits<float>::max();
	if (mActorOwner)
	{
		const FName TargetName = mActorOwner->ActorHasTag("Player") ? TEXT("Enemy") : TEXT("Player");

		TArray<AActor*> outList;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetName, outList);

		for (AActor* item : outList)
		{
			if (APDBaseCharacter* TargetCharacter = Cast<APDBaseCharacter>(item))
			{
				if (true == TargetCharacter->IsAlive())
				{
					const float nearDistance = mActorOwner->GetDistanceTo(TargetCharacter);
					if (nearDistance < ClosestDistance)
					{
						ClosestDistance = nearDistance;
						OwnerComp.GetBlackboardComponent()->SetValueAsObject(UPDAIBehaviorComponent::TargetToFollow, TargetCharacter);		
					}
				}
			}
		}

		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(UPDAIBehaviorComponent::DistToTarget, ClosestDistance);
	}
}
