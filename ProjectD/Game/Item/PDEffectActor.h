// Copyright(c) Depsee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PDEffectActor.generated.h"

class UGameplayEffect;
class UBoxComponent;


UCLASS()
class PROJECTD_API APDEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APDEffectActor();


	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> OverlapCollisionBox;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass);


	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGamePlayEffectClass;
};
