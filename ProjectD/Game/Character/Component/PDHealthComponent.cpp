// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Character/Component/PDHealthComponent.h"
#include "Game/PDGameplayTags.h"
#include "Game/Ability/PDHealthSet.h"

#include "GameplayAbilities/Public/GameplayEffectTypes.h"
#include "GameplayAbilities/Public/GameplayEffectExtension.h"

UPDHealthComponent::UPDHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	_DeathState = EPDDeathState::NotDead;

	_HealthData = MakeUnique<FHealthPoint>(100);
}


void UPDHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	Super::OnUnregister();
}

void UPDHealthComponent::InitializeWithAbilitySystem()
{	
}

void UPDHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();
}

float UPDHealthComponent::GetHealth() const
{
	return _HealthData.Get()->GetValue();
}

float UPDHealthComponent::GetMaxHealth() const
{
	return _HealthData.Get()->GetMaxValue();
}

float UPDHealthComponent::GetHealthNormalized() const
{
	float Health = _HealthData.Get()->GetValue();
	float MaxHealth = _HealthData.Get()->GetMaxValue();

	return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
}

void UPDHealthComponent::StartDeath()
{
	if (_DeathState != EPDDeathState::NotDead)
	{
		return;
	}

	_DeathState = EPDDeathState::DeathStarted;

	AActor* Owner = GetOwner();
	check(Owner);

	_OnDeathStarted.Broadcast(Owner);
}

void UPDHealthComponent::FinishDeath()
{
	if (_DeathState != EPDDeathState::DeathStarted)
	{
		return;
	}

	_DeathState = EPDDeathState::DeathFinished;


	AActor* Owner = GetOwner();
	check(Owner);

	_OnDeathFinished.Broadcast(Owner);
}

void UPDHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld /*= false*/)
{
}



void UPDHealthComponent::ClearGameplayTags()
{

}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	return nullptr;
}
