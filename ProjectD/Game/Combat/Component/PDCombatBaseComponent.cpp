// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Combat/Component/PDCombatBaseComponent.h"



UPDCombatBaseComponent::UPDCombatBaseComponent(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UPDCombatBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPDCombatBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

