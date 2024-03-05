// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Player/PDCheatManager.h"

#include "Kismet/KismetInternationalizationLibrary.h"
#include "GameplayEffect.h"
#include "Templates/SubclassOf.h"
#include "GameplayEffectTypes.h"

#include "Game/Player/PDPlayerController.h"
#include "Game/Character/Component/PDPlayerLevelManagerComponent.h"
#include "Game/PDGameplayTags.h"
#include "Game/System/PDGameData.h"
#include "Game/System/PDAssetManager.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"


void UPDCheatManager::AddPlayerExpr(float value)
{
	if (APDPlayerController* PlayerController = Cast<APDPlayerController>(GetOuterAPlayerController()))
	{
		if (const auto pawn = UPDPlayerLevelManagerComponent::FindPlayerLevelMangerComponent(PlayerController->GetPawn()))
		{
			pawn->AddExperience(value);
		}
	}
}

void UPDCheatManager::ChangeLanguage(const FString& language)
{
	UKismetInternationalizationLibrary::SetCurrentCulture(language);
}

void UPDCheatManager::DamageSelf(float DamageAmount)
{
	if (UPDAbilitySystemComponent* LyraASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerDamage(LyraASC, DamageAmount);
	}
}

void UPDCheatManager::HealSelf(float HealAmount)
{
	if (UPDAbilitySystemComponent* LyraASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerHeal(LyraASC, HealAmount);
	}
}

void UPDCheatManager::ApplySetByCallerDamage(UPDAbilitySystemComponent* PDASC, float DamageAmount)
{
	check(PDASC);

	TSubclassOf<UGameplayEffect> DamageGE = UPDAssetManager::GetSubclass(UPDGameData::Get().DamageGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = PDASC->MakeOutgoingSpec(DamageGE, 1.0f, PDASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(PDGameplayTags::SetByCaller_Damage, DamageAmount);
		PDASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UPDCheatManager::ApplySetByCallerHeal(UPDAbilitySystemComponent* PDASC, float HealAmount)
{
	check(PDASC);

	TSubclassOf<UGameplayEffect> HealGE = UPDAssetManager::GetSubclass(UPDGameData::Get().HealGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = PDASC->MakeOutgoingSpec(HealGE, 1.0f, PDASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(PDGameplayTags::SetByCaller_Heal, HealAmount);
		PDASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

UPDAbilitySystemComponent* UPDCheatManager::GetPlayerAbilitySystemComponent() const
{
	if (APDPlayerController* LyraPC = Cast<APDPlayerController>(GetOuterAPlayerController()))
	{
		return LyraPC->GetPDAbilitySystemComponent();
	}
	return nullptr;
}
