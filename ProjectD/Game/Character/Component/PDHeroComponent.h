// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"

#include "Game/GameFeatures/GameFeatureAction_AddInputContextMapping.h"
#include "GameplayAbilitySpecHandle.h"
#include "PDHeroComponent.generated.h"

class UPDCameraMode;
class UPDInputConfig;
struct FMappableConfigPair;
struct FInputActionValue;

//
// 플레이어 Controllered Pawn(또는 AI)에 대한 입력(Input)및 카메라 처리(Cam)를 설정하는 컴포넌트입니다.
//  이는 초기화를 조정하기 위해 PDPawnExtensionComponent 에 따라 달라진다.
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class PROJECTD_API UPDHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UPDHeroComponent(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintPure, Category = "PD|Hero")
	static UPDHeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPDHeroComponent>() : nullptr); }

	
	void SetAbilityCameraMode(TSubclassOf<UPDCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);	
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);


	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;


	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface


		/** Adds mode-specific input config */
	void AddAdditionalInputConfig(const UPDInputConfig* InputConfig);

	/** Removes a mode-specific input config if it has been added */
	void RemoveAdditionalInputConfig(const UPDInputConfig* InputConfig);


	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	bool IsReadyToBindInputs() const;
protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);


	TSubclassOf<UPDCameraMode> DetermineCameraMode() const;

protected:
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);


protected:
	/** Spec handle for the last ability to set a camera mode. */
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;


	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> _DefaultInputMappings;


	/** 기능에 의해 설정된 카메라 모드 */
	UPROPERTY()
	TSubclassOf<UPDCameraMode> _AbilityCameraMode;

	/** True when player input bindings have been applied, will never be true for non - players */
	bool _bReadyToBindInputs;
};
