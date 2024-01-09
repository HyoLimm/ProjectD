// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/Ability/PDHealthSet.h"
#include "PDPlayerLevelManagerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPDExpr_ChangeEvent, float, ExprValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPDLevelUpEvent);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTD_API UPDPlayerLevelManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPDPlayerLevelManagerComponent();

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "PD|Experience")
	static UPDPlayerLevelManagerComponent* FindPlayerLevelMangerComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPDPlayerLevelManagerComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable, Category = "PD|Experience")
	float GetExprNormalized() const;

	UPROPERTY(BlueprintAssignable)
	FPDExpr_ChangeEvent OnExprChanged;

	UPROPERTY(BlueprintAssignable)
	FPDLevelUpEvent OnLeveledUp;
public:
	UFUNCTION(BlueprintCallable, Category = "PD|Experience")
	const float	GetExp() const;

	UFUNCTION(BlueprintCallable, Category = "PD|Experience")
	const float	GetMaxExp() const;


	UFUNCTION(BlueprintCallable, Category = "PD|Experience")
	void		AddExperience(const float amount);


	UFUNCTION(BlueprintCallable, Category = "PD|Experience")
	void		SetMaxExpr(const float amount);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void		LevelUp();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	TUniquePtr<FExperiencePoint> _ExperienceData;
};
