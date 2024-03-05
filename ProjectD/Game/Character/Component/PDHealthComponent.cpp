// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Character/Component/PDHealthComponent.h"
#include "Game/PDGameplayTags.h"
#include "Game/Messages/PDVerbMessageHelpers.h"


#include "GameplayAbilities/Public/GameplayEffectTypes.h"
#include "GameplayAbilities/Public/GameplayEffectExtension.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDHealthComponent)
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_PD_Elimination_Message, "PD.Elimination.Message");

UPDHealthComponent::UPDHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	_AbilitySystemComponent = nullptr;
	_HealthSet = nullptr;
	_DeathState = EPDDeathState::NotDead;
}


void UPDHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	Super::OnUnregister();
}

void UPDHealthComponent::InitializeWithAbilitySystem(UPDAbilitySystemComponent* InASC)
{	
	AActor* Owner = GetOwner();
	check(Owner);

	if (_AbilitySystemComponent)
	{
		UE_LOG(LogPD, Error, TEXT("PDHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	_AbilitySystemComponent = InASC;
	if (nullptr == _AbilitySystemComponent)
	{
		check(false);
		UE_LOG(LogPD, Error, TEXT("PDHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	_HealthSet = _AbilitySystemComponent->GetSet<UPDHealthSet>();
	if (!_HealthSet)
	{
		check(false);
		UE_LOG(LogPD, Error, TEXT("PDHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	_HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	_HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	_HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	// 임시 : 속성을 기본값으로 재설정합니다. 결국 스프레드시트에 의해 구동됩니다.
	_AbilitySystemComponent->SetNumericAttributeBase(UPDHealthSet::GetHealthAttribute(), _HealthSet->GetMaxHealth()/2);

	ClearGameplayTags();

	const float curHealth = _HealthSet->GetHealth();

	OnHealthChanged.Broadcast(this, curHealth, curHealth, nullptr);
	OnMaxHealthChanged.Broadcast(this, curHealth, curHealth, nullptr);
}

void UPDHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (_HealthSet)
	{
		_HealthSet->OnHealthChanged.RemoveAll(this);
		_HealthSet->OnMaxHealthChanged.RemoveAll(this);
		_HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	_HealthSet = nullptr;
	_AbilitySystemComponent = nullptr;
}

float UPDHealthComponent::GetHealth() const
{
	return (_HealthSet ? _HealthSet->GetHealth() : 0.0f);
}

float UPDHealthComponent::GetMaxHealth() const
{
	return (_HealthSet ? _HealthSet->GetMaxHealth() : 0.0f);
}

float UPDHealthComponent::GetHealthNormalized() const
{
	if (_HealthSet)
	{
		const float Health = _HealthSet->GetHealth();
		const float MaxHealth = _HealthSet->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}

void UPDHealthComponent::StartDeath()
{
	if (_DeathState != EPDDeathState::NotDead)
	{
		return;
	}

	_DeathState = EPDDeathState::DeathStarted;

	if (_AbilitySystemComponent)
	{
		_AbilitySystemComponent->SetLooseGameplayTagCount(PDGameplayTags::Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);
}

void UPDHealthComponent::FinishDeath()
{
	if (_DeathState != EPDDeathState::DeathStarted)
	{
		return;
	}

	_DeathState = EPDDeathState::DeathFinished;

	if (_AbilitySystemComponent)
	{
		_AbilitySystemComponent->SetLooseGameplayTagCount(PDGameplayTags::Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);
}

void UPDHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld /*= false*/)
{
	if ((_DeathState == EPDDeathState::NotDead) && _AbilitySystemComponent)
	{
		const TSubclassOf<UGameplayEffect> DamageGE = UPDAssetManager::GetSubclass(UPDGameData::Get().DamageGameplayEffect_SetByCaller);
		if (!DamageGE)
		{
			UE_LOG(LogPD, Error, TEXT("PDHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to find gameplay effect [%s]."), *GetNameSafe(GetOwner()), *UPDGameData::Get().DamageGameplayEffect_SetByCaller.GetAssetName());
			return;
		}

		FGameplayEffectSpecHandle SpecHandle = _AbilitySystemComponent->MakeOutgoingSpec(DamageGE, 1.0f, _AbilitySystemComponent->MakeEffectContext());
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		if (!Spec)
		{
			UE_LOG(LogPD, Error, TEXT("PDHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to make outgoing spec for [%s]."), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
			return;
		}

		Spec->AddDynamicAssetTag(TAG_Gameplay_DamageSelfDestruct);

		if (bFellOutOfWorld)
		{
			Spec->AddDynamicAssetTag(TAG_Gameplay_FellOutOfWorld);
		}

		const float DamageAmount = GetMaxHealth();

		Spec->SetSetByCallerMagnitude(PDGameplayTags::SetByCaller_Damage, DamageAmount);
		_AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}



void UPDHealthComponent::ClearGameplayTags()
{
	if (_AbilitySystemComponent)
	{
		_AbilitySystemComponent->SetLooseGameplayTagCount(PDGameplayTags::Status_Death_Dying, 0);
		_AbilitySystemComponent->SetLooseGameplayTagCount(PDGameplayTags::Status_Death_Dead, 0);
	}
}

void UPDHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UPDHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UPDHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	if (_AbilitySystemComponent && DamageEffectSpec)
	{
		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		{
			FGameplayEventData Payload;
			Payload.EventTag = PDGameplayTags::GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = _AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec->Def;
			Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(_AbilitySystemComponent, true);
			_AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		// Send a standardized verb message that other systems can observe
		{
			FPDVerbMessage Message;
			Message.Verb = TAG_PD_Elimination_Message;
			Message.Instigator = DamageInstigator;
			Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Message.Target = UPDVerbMessageHelpers::GetPlayerStateFromObject(_AbilitySystemComponent->GetAvatarActor());
			Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		//@TODO: assist messages (could compute from damage dealt elsewhere)?
	}

#endif // #if WITH_SERVER_CODE
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	return nullptr;
}
