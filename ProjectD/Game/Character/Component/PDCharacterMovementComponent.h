// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PDCharacterMovementComponent.generated.h"

/**
 * FPDCharacterGroundInfo
 *
 *	Information about the ground under the character.  It only gets updated as needed.
 */
USTRUCT(BlueprintType)
struct FPDCharacterGroundInfo
{
	GENERATED_BODY()

	FPDCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


UCLASS()
class PROJECTD_API UPDCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:

	UPDCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);


	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "PD|CharacterMovement")
	const FPDCharacterGroundInfo& GetGroundInfo();


	//~UMovementComponent interface
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface
protected:

	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FPDCharacterGroundInfo CachedGroundInfo;
};
