// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "PDAttributeSet.h"
#include "NativeGameplayTags.h"

#include "PDHealthSet.generated.h"

class UObject;
struct FFrame;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_PD_Damage_Message);

struct FGameplayEffectModCallbackData;


/**
 * UPDHealthSet
 *
 *	Damage을 입히는 데 필요한 특성을 정의하는 클래스입니다.	
 *  Attribute ex)  쉴드 및 저항 등이 포함됩니다.
 */
UCLASS(BlueprintType)
class UPDHealthSet : public UPDAttributeSet
{
	GENERATED_BODY()
public:
	UPDHealthSet();

	mutable FPDAttributeEvent OnHealthChanged;
	mutable FPDAttributeEvent OnMaxHealthChanged;	
	mutable FPDAttributeEvent OnOutOfHealth;


	ATTRIBUTE_ACCESSORS(UPDHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UPDHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UPDHealthSet, Damage);
	ATTRIBUTE_ACCESSORS(UPDHealthSet, Healing);
protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	//~UAttributeSet interface
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	//~End of UAttributeSet interface


	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "PD|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "PD|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;



	// 체력이 0에 도달하면 추적하는데 사용 된다
	bool _bOutOfHealth;

	// health 변경하기전에 이전 값 저장용
	float _MaxHealthBeforeAttributeChange;
	float _HealthBeforeAttributeChange;

	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------	
	UPROPERTY(BlueprintReadOnly, Category="PD|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;


	
	UPROPERTY(BlueprintReadOnly, Category="PD|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;

};
