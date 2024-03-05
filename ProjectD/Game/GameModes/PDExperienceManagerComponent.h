// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "PDExperienceManagerComponent.generated.h"



namespace UE::GameFeatures { struct FResult; }


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


	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface



	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnPDExperienceLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded(FOnPDExperienceLoaded::FDelegate&& Delegate);


	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnPDExperienceLoaded::FDelegate&& Delegate);

	// Tries to set the current experience, either a UI or gameplay one
	// UI �Ǵ� ���� �÷��� �� �ϳ��� ���� ������ �����Ϸ��� �մϴ�
	void SetCurrentExperience(const FPrimaryAssetId& ExperienceId);

	// ����ġ�� ������ �ε�� ��� true�� ��ȯ�մϴ�.
	bool IsExperienceLoaded() const;



	// This returns the current experience if it is fully loaded, asserting otherwise
	// �̰��� ���� ������ ������ �ε�� ��� ���� ������ ��ȯ�ϰ� �׷��� ���� ��� �����մϴ�.
// (i.e., if you called it too soon)
	const UPDExperienceDefinition* GetCurrentExperienceChecked() const;

private:
	UPROPERTY()
	TObjectPtr<const UPDExperienceDefinition> _CurrentExperience;


	EPDExperienceLoadState _LoadState = EPDExperienceLoadState::Unloaded;

	/**
	 * Delegate called when the experience has finished loading just before others
	 * (e.g., subsystems that set up for regular gameplay)
	 */
	FOnPDExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading */
	FOnPDExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading */
	FOnPDExperienceLoaded OnExperienceLoaded_LowPriority;

private:
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();


	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

	int32 NumGameFeaturePluginsLoading = 0;

	TArray<FString> GameFeaturePluginURLs;
};
