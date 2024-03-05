// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Combat/Component/PDPlayerCombatComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Game/Physics/PDCollisionChannels.h"

#include "../../../../../../../Source/Runtime/Engine/Classes/Engine/EngineTypes.h"

UPDPlayerCombatComponent::UPDPlayerCombatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IsAutoAttack = false;
	//SetIsReplicatedByDefault(true);
}




void UPDPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();


	FTimerHandle NextTimeToReconsiderHandle;

	//GetWorld()->GetTimerManager().SetTimer(NextTimeToReconsiderHandle, this, &ThisClass::TraceNearEnemy, 2.0f,true);
}

void UPDPlayerCombatComponent::UpdateAttackWeaponList()
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return;
	}

	if (UPDInventoryManagerComponent* InventoryManagerCompo = UPDInventoryManagerComponent::FindInventoryManagerComponent(OwningActor))
	{
		InventoryWeaponList = InventoryManagerCompo->GetAllItems();
	}
}

FVector UPDPlayerCombatComponent::TraceNearEnemy()
{
	AController* OwningActor = GetOwner<AController>();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return FVector::ZeroVector;
	}
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwningActor->GetPawn());

	TArray<FHitResult> OutHits;
	FVector PawnLocation = OwningActor->GetPawn()->GetActorLocation();

	ETraceTypeQuery traceType = UEngineTypes::ConvertToTraceType(PD_TraceChannel_Weapon);
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), PawnLocation, PawnLocation,1000.0f, traceType,false, IgnoreActors,EDrawDebugTrace::ForDuration, OutHits,true,FLinearColor::Red,FLinearColor::Green,5.0f);

	float mindist = 999999.0f;

	AActor* tragetActor = nullptr;
	for (const FHitResult HitTarget : OutHits)
	{
		float dist = FVector::Distance(HitTarget.GetActor()->GetActorLocation(), PawnLocation);
		if (dist <= mindist)
		{
			tragetActor=HitTarget.GetActor(); 
			mindist = dist;
		}
	}
	

	if(tragetActor)
	{
		DrawDebugLine(GetWorld(), PawnLocation, tragetActor->GetActorLocation(), FColor::Blue,false,3.0f,0,2.0f);
		DrawDebugSphere(GetWorld(), tragetActor->GetActorLocation(), 50.0f, 16, FColor::Cyan,false,3.0f,0,2.0f);		

		return tragetActor->GetActorLocation();
	}

	//check(false);
	return PawnLocation;
}
