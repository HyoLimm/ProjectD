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
// 기능이 추적을 시작하는 위치와 해당 기능이 마주해야 하는 위치를 정의합니다
UENUM(BlueprintType)
enum class EPDAbilityTargetingSource : uint8
{
	// From the player's camera towards camera focus // 플레이어 카메라 -> 카메라 포커스 초점
	CameraTowardsFocus,
	// From the pawn's center, in the pawn's orientation/  Pawn 중앙 -> pawn
	PawnForward,
	// From the pawn's center, oriented towards camera focus // Pawn 중앙 -> 카메라 포커스
	PawnTowardsFocus,
	// From the weapon's muzzle or location, in the pawn's orientation // 무기 위치 -> 폰 방향
	WeaponForward,
	// From the weapon's muzzle or location, towards camera focus //무기 위치->카메라 포커스
	WeaponTowardsFocus,
	// Custom blueprint-specified source location
	Custom,

	TraceNearEnemyForward,
};



/**
 * UPDGameplayAbility_RangedWeapon
 *
 * An ability granted by and associated with a ranged weapon instance
 * 원거리 무기 인스턴스에 의해 부여되고 이와 연관된 능력
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
