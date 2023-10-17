// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameModes/PDExperienceManagerComponent.h"
#include "PDExperienceDefinition.h"
#include "Game/System/PDAssetManager.h"


namespace PDConsoleVariables
{
	static float ExperienceLoadRandomDelayMin = 0.0f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayMin(
		TEXT("pd.chaos.ExperienceDelayLoad.MinSecs"),
		ExperienceLoadRandomDelayMin,
		TEXT("This value (in seconds) will be added as a delay of load completion of the experience (along with the random value PD.chaos.ExperienceDelayLoad.RandomSecs)"),
		ECVF_Default);

	static float ExperienceLoadRandomDelayRange = 0.0f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayRange(
		TEXT("pd.chaos.ExperienceDelayLoad.RandomSecs"),
		ExperienceLoadRandomDelayRange,
		TEXT("A random amount of time between 0 and this value (in seconds) will be added as a delay of load completion of the experience (along with the fixed value PD.chaos.ExperienceDelayLoad.MinSecs)"),
		ECVF_Default);

	float GetExperienceLoadDelayDuration()
	{
		return FMath::Max(0.0f, ExperienceLoadRandomDelayMin + FMath::FRand() * ExperienceLoadRandomDelayRange);
	}
}


UPDExperienceManagerComponent::UPDExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//SetIsReplicatedByDefault(true);
}



bool UPDExperienceManagerComponent::IsExperienceLoaded() const
{
	return (LoadState == EPDExperienceLoadState::Loaded) && (CurrentExperience != nullptr);
}


const UPDExperienceDefinition* UPDExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EPDExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}

void UPDExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience != nullptr);
	check(LoadState == EPDExperienceLoadState::Unloaded);
	LoadState = EPDExperienceLoadState::Loading;


	UPDAssetManager& AssetManager = UPDAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;

	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());
	//for (const TObjectPtr<UPDExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	//{
	//	if (ActionSet != nullptr)
	//	{
	//		BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
	//	}
	//}

	// Load assets associated with the experience

	TArray<FName> BundlesToLoad;
	//BundlesToLoad.Add(FPDBundles::Equipped);

	////@TODO: Centralize this client/server stuff into the PDAssetManager
	//const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	//const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
	//const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
	//if (bLoadClient)
	//{
	//	BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	//}
	//if (bLoadServer)
	//{
	//	BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	//}

	TSharedPtr<FStreamableHandle> BundleLoadHandle = nullptr;
	if (BundleAssetList.Num() > 0)
	{
		BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
	}

	TSharedPtr<FStreamableHandle> RawLoadHandle = nullptr;
	if (RawAssetList.Num() > 0)
	{
		RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(), FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority, TEXT("StartExperienceLoad()"));
	}

	// If both async loads are running, combine them
	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}
	else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

	//OnExperienceLoadComplete();
	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// Assets were already loaded, call the delegate now
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);

		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
			{
				OnAssetsLoadedDelegate.ExecuteIfBound();
			}));
	}

	// This set of assets gets preloaded, but we don't block the start of the experience based on it
	//TSet<FPrimaryAssetId> PreloadAssetList;
	////@TODO: Determine assets to preload (but not blocking-ly)
	//if (PreloadAssetList.Num() > 0)
	//{
	//	AssetManager.ChangeBundleStateForPrimaryAssets(PreloadAssetList.Array(), BundlesToLoad, {});
	//}
}

void UPDExperienceManagerComponent::OnExperienceLoadComplete()
{
	check(LoadState == EPDExperienceLoadState::Loading);
	check(CurrentExperience != nullptr);

	OnExperienceFullLoadCompleted();
}
//
//void UPDExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
//{
//
//}

void UPDExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != EPDExperienceLoadState::Loaded);

	//// Insert a random delay for testing (if configured)
	//if (LoadState != EPDExperienceLoadState::LoadingChaosTestingDelay)
	//{
	//	const float DelaySecs = PDConsoleVariables::GetExperienceLoadDelayDuration();
	//	if (DelaySecs > 0.0f)
	//	{
	//		FTimerHandle DummyHandle;

	//		LoadState = EPDExperienceLoadState::LoadingChaosTestingDelay;
	//		GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &ThisClass::OnExperienceFullLoadCompleted, DelaySecs, /*bLooping=*/ false);

	//		return;
	//	}
	//}

	//LoadState = EPDExperienceLoadState::ExecutingActions;

	// Execute the actions
	//FGameFeatureActivatingContext Context;

	//// Only apply to our specific world context if set
	//const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	//if (ExistingWorldContext)
	//{
	//	Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	//}

	//auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
	//	{
	//		for (UGameFeatureAction* Action : ActionList)
	//		{
	//			if (Action != nullptr)
	//			{
	//				//@TODO: The fact that these don't take a world are potentially problematic in client-server PIE
	//				// The current behavior matches systems like gameplay tags where loading and registering apply to the entire process,
	//				// but actually applying the results to actors is restricted to a specific world
	//				Action->OnGameFeatureRegistering();
	//				Action->OnGameFeatureLoading();
	//				Action->OnGameFeatureActivating(Context);
	//			}
	//		}
	//	};

	//ActivateListOfActions(CurrentExperience->Actions);
	//for (const TObjectPtr<UPDExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	//{
	//	if (ActionSet != nullptr)
	//	{
	//		ActivateListOfActions(ActionSet->Actions);
	//	}
	//}

	LoadState = EPDExperienceLoadState::Loaded;

//	OnExperienceLoaded_HighPriority.Broadcast(CurrentExperience);
//	OnExperienceLoaded_HighPriority.Clear();
//
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
//
//	OnExperienceLoaded_LowPriority.Broadcast(CurrentExperience);
//	OnExperienceLoaded_LowPriority.Clear();
//
//	// Apply any necessary scalability settings
//#if !UE_SERVER
//	UPDSettingsLocal::Get()->OnExperienceLoaded();
//#endif
}

void UPDExperienceManagerComponent::OnActionDeactivationCompleted()
{

}

void UPDExperienceManagerComponent::OnAllActionsDeactivated()
{

}

void UPDExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnPDExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UPDExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)
{	
	const FSoftObjectPath AssetPath = UPDAssetManager::Get().GetPrimaryAssetPath(ExperienceId);

	TSubclassOf<UPDExperienceDefinition> AssetClass = Cast<UClass>(AssetPath.TryLoad());
	check(AssetClass);
	const UPDExperienceDefinition* Experience = GetDefault<UPDExperienceDefinition>(AssetClass);

	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	CurrentExperience = Experience;
	StartExperienceLoad();
}
