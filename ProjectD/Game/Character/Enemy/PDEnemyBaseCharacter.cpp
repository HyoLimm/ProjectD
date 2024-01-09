// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Character/Enemy/PDEnemyBaseCharacter.h"

APDEnemyBaseCharacter::APDEnemyBaseCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	BehaviorComponent = CreateDefaultSubobject<UPDAIBehaviorComponent>(TEXT("AIBehaviorComponent"));
}
