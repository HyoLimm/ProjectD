// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "PDHeroComponent.generated.h"

class UPDCameraMode;
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


protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);


	TSubclassOf<UPDCameraMode> DetermineCameraMode() const;

protected:
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
protected:

	UPROPERTY(EditAnywhere)
	TArray<FMappableConfigPair> DefaultInputConfigs;


	/** 기능에 의해 설정된 카메라 모드 */
	UPROPERTY()
	TSubclassOf<UPDCameraMode> AbilityCameraMode;

	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs;
};
