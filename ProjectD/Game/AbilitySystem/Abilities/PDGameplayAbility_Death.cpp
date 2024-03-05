// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDGameplayAbility_Death.h"

#include "Game/AbilitySystem/Abilities/PDGameplayAbility.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"
#include "Game/Character/Component/PDHealthComponent.h"
#include "Game/PDGameplayTags.h"
#include "Game/PDLogChannels.h"
#include "Trace/Trace.inl"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDGameplayAbility_Death)

UPDGameplayAbility_Death::UPDGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	bAutoStartDeath = true;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = PDGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UPDGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	UPDAbilitySystemComponent* PDASC = CastChecked<UPDAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(PDGameplayTags::Ability_Behavior_SurvivesDeath);

	// Cancel all abilities and block others from starting.
	PDASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	if (!ChangeActivationGroup(EPDAbilityActivationGroup::Exclusive_Blocking))
	{
		UE_LOG(LogPDAbilitySystem, Error, TEXT("UPDGameplayAbility_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	}

	if (bAutoStartDeath)
	{
		StartDeath();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UPDGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	// Always try to finish the death when the ability ends in case the ability doesn't.
	// This won't do anything if the death hasn't been started.
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPDGameplayAbility_Death::StartDeath()
{
	if (UPDHealthComponent* HealthComponent = UPDHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EPDDeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

void UPDGameplayAbility_Death::FinishDeath()
{
	if (UPDHealthComponent* HealthComponent = UPDHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EPDDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}

