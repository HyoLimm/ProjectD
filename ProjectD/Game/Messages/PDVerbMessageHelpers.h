// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "PDVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FPDVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class PROJECTD_API UPDVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PD")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "PD")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "PD")
	static FGameplayCueParameters VerbMessageToCueParameters(const FPDVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "PD")
	static FPDVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
