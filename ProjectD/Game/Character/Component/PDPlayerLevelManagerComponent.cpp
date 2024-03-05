// Fill out your copyright notice in the Description page of Project Settings.
#include "Game/Character/Component/PDPlayerLevelManagerComponent.h"

// Sets default values for this component's properties
UPDPlayerLevelManagerComponent::UPDPlayerLevelManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	//_ExperienceData = MakeUnique<FExperiencePoint>(0);
	//_ExperienceData.Get()->LevelUpDelegate.BindUObject(this,ThisClass::LevelUp());
	
	
}


float UPDPlayerLevelManagerComponent::GetExprNormalized() const
{
	//if (_ExperienceData.Get())
	//{
	//	const float Health = _ExperienceData.Get()->GetValue();
	//	const float MaxHealth = _ExperienceData.Get()->GetMaxValue();

	//	return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	//}

	return 0.0f;
}

// Called when the game starts
void UPDPlayerLevelManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	//HYOTODO 경험치 맥스 설정
	SetMaxExpr(1000.0f);
	
}


const float UPDPlayerLevelManagerComponent::GetMaxExp() const
{
	return 0.0f;
}

const float UPDPlayerLevelManagerComponent::GetExp() const
{
	return 0.0f;
}

void UPDPlayerLevelManagerComponent::AddExperience(const float amount)
{
	//if (true == _ExperienceData.Get()->AddExpr(amount))
	//{		
	//	OnExprChanged.Broadcast(0.0f);
	//	LevelUp();
	//}
	//else
	//{
	//	OnExprChanged.Broadcast(amount);
	//}

}

void UPDPlayerLevelManagerComponent::SetMaxExpr(const float amount)
{
	//_ExperienceData.Get()->SetMax(amount);
}

void UPDPlayerLevelManagerComponent::LevelUp()
{
	//_ExperienceData.Get()->GetMaxValue();
	//HYOTODO 레벨업하면 max 수치 올려야됌
	// SetMaxExpr();	

	OnLeveledUp.Broadcast();

	//OnExprChanged.Broadcast(_ExperienceData.Get()->GetValue());
}

// Called every frame
void UPDPlayerLevelManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

