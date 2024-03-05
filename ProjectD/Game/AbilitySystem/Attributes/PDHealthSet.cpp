// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDHealthSet.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

#include "GameplayEffectExtension.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "Game/AbilitySystem/Attributes/PDAttributeSet.h"
#include "Game/PDGameplayTags.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"
#include "Game/Messages/PDVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDHealthSet)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity"); // Immunity 면역
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_PD_Damage_Message, "PD.Damage.Message");

UPDHealthSet::UPDHealthSet()
	: Health(10.0f)
	, MaxHealth(100.0f)
{
	_bOutOfHealth = false;
	_MaxHealthBeforeAttributeChange = 0.0f;
	_HealthBeforeAttributeChange = 0.0f;
}

void UPDHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPDHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPDHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UPDHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPDHealthSet, Health, OldValue);

	const float CurrentHealth = GetHealth();
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();
	
	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

	if (!_bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	}

	_bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UPDHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPDHealthSet, MaxHealth, OldValue);

	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxHealth());
}

bool UPDHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Handle modifying 받는 일반 데미지
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);

			if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}

#if !UE_BUILD_SHIPPING
			// Check GodMode cheat, unlimited health is checked below
			if (Data.Target.HasMatchingGameplayTag(PDGameplayTags::Cheat_GodMode) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
#endif // #if !UE_BUILD_SHIPPING
		}
	}

	// Save the current health
	_HealthBeforeAttributeChange = GetHealth();
	_MaxHealthBeforeAttributeChange = GetMaxHealth();

	return true;
}

void UPDHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);
	float MinimumHealth = 0.0f;

#if !UE_BUILD_SHIPPING
	// Godmode and unlimited health stop death unless it's a self destruct
	if (!bIsDamageFromSelfDestruct &&
		(Data.Target.HasMatchingGameplayTag(PDGameplayTags::Cheat_GodMode) || Data.Target.HasMatchingGameplayTag(PDGameplayTags::Cheat_UnlimitedHealth) ))
	{
		MinimumHealth = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Send a standardized verb message that other systems can observe
		// 다른 시스템에서 관찰할 수 있는 표준화된 동사 메시지 전송
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			FPDVerbMessage Message;
			Message.Verb = TAG_PD_Damage_Message;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		// Convert into -Health and then clamp
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Convert into +Health and then clamo
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// TODO clamp current health?

		// 최대 체력 변경 되면 broadcast
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, _MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// health가 변경된 경우 broadcast
	if (GetHealth() != _HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, _HealthBeforeAttributeChange, GetHealth());
	}

	// 체력이 0이 됐을 경우
	if ((GetHealth() <= 0.0f) && !_bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, _HealthBeforeAttributeChange, GetHealth());
	}
	
	// 이벤트가 변경된 경우를 대비하여 health 상태를 다시 확인합니다.
	_bOutOfHealth = (GetHealth() <= 0.0f);
}

void UPDHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPDHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPDHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{		
		// 현재 체력이 새 최대 체력보다 크지 않은지 확인합니다.
		if (GetHealth() > NewValue)
		{
			UPDAbilitySystemComponent* PDASC = GetPDAbilitySystemComponent();
			check(PDASC);

			PDASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (_bOutOfHealth && (GetHealth() > 0.0f))
	{
		_bOutOfHealth = false;
	}
}

void UPDHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

