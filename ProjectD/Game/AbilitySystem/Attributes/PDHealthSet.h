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
 *	Damage�� ������ �� �ʿ��� Ư���� �����ϴ� Ŭ�����Դϴ�.	
 *  Attribute ex)  ���� �� ���� ���� ���Ե˴ϴ�.
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



	// ü���� 0�� �����ϸ� �����ϴµ� ��� �ȴ�
	bool _bOutOfHealth;

	// health �����ϱ����� ���� �� �����
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
