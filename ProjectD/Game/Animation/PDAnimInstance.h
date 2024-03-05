// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "PDAnimInstance.generated.h"

class UAbilitySystemComponent;


/**
 * UPDAnimInstance
 *
 *	The base game animation instance class used by this project.
 */
UCLASS(Config = Game)
class UPDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPDAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// Blueprint ������ ������ �� �ִ� ���� �÷��� �±��Դϴ�.�±װ� �߰��ǰų� ���ŵǸ� ������ �ڵ����� ������Ʈ�˴ϴ�.
	// ���� �÷��� �±׸� �������� ��ȸ�ϴ� ��� ����ؾ� �մϴ�.
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
};
