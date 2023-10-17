// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "PDExperienceManagerComponent.generated.h"
class UPDExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPDExperienceLoaded, const UPDExperienceDefinition* /*Experience*/);


class UPDExperienceDefinition;


enum class EPDExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};


UCLASS()
class PROJECTD_API UPDExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
public:

	UPDExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded(FOnPDExperienceLoaded::FDelegate&& Delegate);

	// Tries to set the current experience, either a UI or gameplay one
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	// ����ġ�� ������ �ε�� ��� true�� ��ȯ�մϴ�.
	bool IsExperienceLoaded() const;



	// This returns the current experience if it is fully loaded, asserting otherwise
	// �̰��� ���� ������ ������ �ε�� ��� ���� ������ ��ȯ�ϰ� �׷��� ���� ��� �����մϴ�.
// (i.e., if you called it too soon)
	const UPDExperienceDefinition* GetCurrentExperienceChecked() const;

private:
	UPROPERTY()
	TObjectPtr<const UPDExperienceDefinition> CurrentExperience;


	EPDExperienceLoadState LoadState = EPDExperienceLoadState::Unloaded;



	/** Delegate called when the experience has finished loading */
	FOnPDExperienceLoaded OnExperienceLoaded;

private:
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	//void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();
};
