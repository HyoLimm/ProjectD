// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Character/PDBaseCharacter.h"
#include "PDEnemyBaseCharacter.generated.h"

class UPDAIBehaviorComponent;
UCLASS()
class PROJECTD_API APDEnemyBaseCharacter : public APDBaseCharacter
{
	GENERATED_BODY()
	
public:
	APDEnemyBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PD| EnemyCharacter", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDAIBehaviorComponent> BehaviorComponent;	
};
