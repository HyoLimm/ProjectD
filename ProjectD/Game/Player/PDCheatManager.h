// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "PDCheatManager.generated.h"

class UPDAbilitySystemComponent;

#ifndef USING_CHEAT_MANAGER
#define USING_CHEAT_MANAGER (1 && !UE_BUILD_SHIPPING)
#endif // #ifndef USING_CHEAT_MANAGER

UCLASS(config = Game, Within = PlayerController)
class PROJECTD_API UPDCheatManager : public UCheatManager
{
	GENERATED_BODY()
public:
	
	UFUNCTION(Exec)
	 void AddPlayerExpr(float value);


	 UFUNCTION(Exec)
	 void ChangeLanguage(const FString& language);


	 UFUNCTION(Exec, BlueprintAuthorityOnly)
	 virtual void DamageSelf(float DamageAmount);

	 UFUNCTION(Exec, BlueprintAuthorityOnly)
	 virtual void HealSelf(float HealAmount);

protected:
	void ApplySetByCallerDamage(UPDAbilitySystemComponent* PDASC, float DamageAmount);
	void ApplySetByCallerHeal(UPDAbilitySystemComponent* PDASC, float HealAmount);


	UPDAbilitySystemComponent* GetPlayerAbilitySystemComponent() const;
};
