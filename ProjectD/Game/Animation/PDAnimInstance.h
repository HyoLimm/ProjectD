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
	// Blueprint 변수에 매핑할 수 있는 게임 플레이 태그입니다.태그가 추가되거나 제거되면 변수가 자동으로 업데이트됩니다.
	// 게임 플레이 태그를 수동으로 조회하는 대신 사용해야 합니다.
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
};
