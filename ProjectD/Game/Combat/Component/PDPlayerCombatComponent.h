// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Combat/Component/PDCombatBaseComponent.h"
#include "Game/Inventory/PDInventoryManagerComponent.h"
#include "PDPlayerCombatComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class PROJECTD_API UPDPlayerCombatComponent : public UPDCombatBaseComponent
{
	GENERATED_BODY()

public:
	UPDPlayerCombatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "PD|PlayerCombat")
	static UPDPlayerCombatComponent* FindPlayerCombatComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPDPlayerCombatComponent>() : nullptr); }

	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable, Category = Inventory)
	void	UpdateAttackWeaponList();

	FVector TraceNearEnemy();
public:
	bool	IsAutoAttack = false;

private:

	FVector				_NearEnemyDir;

	UPROPERTY()
	TArray<UPDInventoryItemInstance*> InventoryWeaponList;
};
