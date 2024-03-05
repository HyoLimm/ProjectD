// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Game/Equipment/PDGameplayAbility_FromEquipment.h"

#include "PDGameplayAbility_RangedWeapon.generated.h"

enum ECollisionChannel : int;

class APawn;
class UPDRangedWeaponInstance;
class UObject;
struct FCollisionQueryParams;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEventData;
struct FGameplayTag;
struct FGameplayTagContainer;

/** Defines where an ability starts its trace from and where it should face */
// ����� ������ �����ϴ� ��ġ�� �ش� ����� �����ؾ� �ϴ� ��ġ�� �����մϴ�
UENUM(BlueprintType)
enum class EPDAbilityTargetingSource : uint8
{
	// From the player's camera towards camera focus // �÷��̾� ī�޶� -> ī�޶� ��Ŀ�� ����
	CameraTowardsFocus,
	// From the pawn's center, in the pawn's orientation/  Pawn �߾� -> pawn
	PawnForward,
	// From the pawn's center, oriented towards camera focus // Pawn �߾� -> ī�޶� ��Ŀ��
	PawnTowardsFocus,
	// From the weapon's muzzle or location, in the pawn's orientation // ���� ��ġ -> �� ����
	WeaponForward,
	// From the weapon's muzzle or location, towards camera focus //���� ��ġ->ī�޶� ��Ŀ��
	WeaponTowardsFocus,
	// Custom blueprint-specified source location
	Custom,

	TraceNearEnemyForward,
};



/**
 * UPDGameplayAbility_RangedWeapon
 *
 * An ability granted by and associated with a ranged weapon instance
 * ���Ÿ� ���� �ν��Ͻ��� ���� �ο��ǰ� �̿� ������ �ɷ�
 */
UCLASS()
class UPDGameplayAbility_RangedWeapon : public UPDGameplayAbility_FromEquipment
{
	GENERATED_BODY()

public:

	UPDGameplayAbility_RangedWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="PD|Ability")
	UPDRangedWeaponInstance* GetWeaponInstance() const;

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of UGameplayAbility interface

protected:
	struct FRangedWeaponFiringInput
	{
		// Start of the trace
		FVector StartTrace;

		// End of the trace if aim were perfect
		FVector EndAim;

		// The direction of the trace if aim were perfect
		FVector AimDir;

		// The weapon instance / source of weapon data
		UPDRangedWeaponInstance* WeaponData = nullptr;

		// Can we play bullet FX for hits during this trace
		bool bCanPlayBulletFX = false;

		FRangedWeaponFiringInput()
			: StartTrace(ForceInitToZero)
			, EndAim(ForceInitToZero)
			, AimDir(ForceInitToZero)
		{
		}
	};

protected:
	static int32 FindFirstPawnHitResult(const TArray<FHitResult>& HitResults);

	// Does a single weapon trace, either sweeping or ray depending on if SweepRadius is above zero
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHitResults) const;

	// Wrapper around WeaponTrace to handle trying to do a ray trace before falling back to a sweep trace if there were no hits and SweepRadius is above zero 
	FHitResult DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHits) const;

	// Traces all of the bullets in a single cartridge
	void TraceBulletsInCartridge(const FRangedWeaponFiringInput& InputData, OUT TArray<FHitResult>& OutHits);

	virtual void AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const;

	// Determine the trace channel to use for the weapon trace(s)
	virtual ECollisionChannel DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const;

	void PerformLocalTargeting(OUT TArray<FHitResult>& OutHits);

	FVector GetWeaponTargetingSourceLocation() const;
	FTransform GetTargetingTransform(APawn* SourcePawn, EPDAbilityTargetingSource Source) const;

	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	UFUNCTION(BlueprintCallable)
	void StartRangedWeaponTargeting();

	// Called when target data is ready
	UFUNCTION(BlueprintImplementableEvent)
	void OnRangedWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

private:
	FDelegateHandle OnTargetDataReadyCallbackDelegateHandle;
};
