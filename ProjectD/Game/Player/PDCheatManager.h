// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "PDCheatManager.generated.h"

/**
 * 
 */
UCLASS(config = Game, Within = PlayerController)
class PROJECTD_API UPDCheatManager : public UCheatManager
{
	GENERATED_BODY()
public:
	
	UFUNCTION(Exec)
	 void AddPlayerExpr(float value);


	 UFUNCTION(Exec)
	 void ChangeLanguage(const FString& language);
};
