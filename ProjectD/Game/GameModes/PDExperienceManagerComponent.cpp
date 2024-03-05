// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameModes/PDExperienceManagerComponent.h"
#include "PDExperienceDefinition.h"
#include "Game/System/PDAssetManager.h"
#include "PDExperienceActionSet.h"
#include "GameFeaturesSubsystem.h"
#include "Game/PDLogChannels.h"
#include "GameFeaturesSubsystemSettings.h"
#include "Game/GameModes/PDExperienceManager.h"
#include "GameFeatureAction.h"


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



void UPDExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// deactivate any features this experience loaded
	//@TODO: This should be handled FILO as well
	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		//if (UPDExperienceManager::RequestToDeactivatePlugin(PluginURL))
		//{
		//	UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		//}
	}

	//@TODO: Ensure proper handling of a partially-loaded state too
	if (_LoadState == EPDExperienceLoadState::Loaded)
	{
		_LoadState = EPDExperienceLoadState::Deactivating;

		// Make sure we won't complete the transition prematurely if someone registers as a pauser but fires immediately
		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;

		// Deactivate and unload the actions
		FGameFeatureDeactivatingContext Context(TEXT(""), [this](FStringView) { this->OnActionDeactivationCompleted(); });

		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (UGameFeatureAction* Action : ActionList)
				{
					if (Action)
					{
						Action->OnGameFeatureDeactivating(Context);
						Action->OnGameFeatureUnregistering();
					}
				}
			};

		DeactivateListOfActions(_CurrentExperience->Actions);
		for (const TObjectPtr<UPDExperienceActionSet>& ActionSet : _CurrentExperience->ActionSets)
		{
			if (ActionSet != nullptr)
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();

		if (NumExpectedPausers > 0)
		{
			UE_LOG(LogPDExperience, Error, TEXT("Actions that have asynchronous deactivation aren't fully supported yet in Lyra experiences"));
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivated();
		}
	}
}

void UPDExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority(FOnPDExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(_CurrentExperience);
	}
	else
	{
		OnExperienceLoaded_HighPriority.Add(MoveTemp(Delegate));
	}
}

bool UPDExperienceManagerComponent::IsExperienceLoaded() const
{
	return (_LoadState == EPDExperienceLoadState::Loaded) && (_CurrentExperience != nullptr);
}


const UPDExperienceDefinition* UPDExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(_LoadState == EPDExperienceLoadState::Loaded);
	check(_CurrentExperience != nullptr);
	return _CurrentExperience;
}

void UPDExperienceManagerComponent::StartExperienceLoad()
{
	check(_CurrentExperience != nullptr);
	check(_LoadState == EPDExperienceLoadState::Unloaded);
	_LoadState = EPDExperienceLoadState::Loading;

	UE_LOG(LogPDExperience, Log, TEXT("EXPERIENCE: StartExperienceLoad(CurrentExperience = %s, %s)"), *_CurrentExperience->GetPrimaryAssetId().ToString(), *GetClientServerContextString(this));

	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;

	BundleAssetList.Add(_CurrentExperience->GetPrimaryAssetId());
	for (const TObjectPtr<UPDExperienceActionSet>& ActionSet : _CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	// Load assets associated with the experience
	// 경험과 관련된 자산 로드
	TArray<FName> BundlesToLoad;
	BundlesToLoad.Add(FPDBundles::Equipped);

	//@TODO: Centralize this client/server stuff into the PDAssetManager
	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
	const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
	if (bLoadClient)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}
	if (bLoadServer)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	UPDAssetManager& AssetManager = UPDAssetManager::Get();
	TSharedPtr<FStreamableHandle> BundleLoadHandle = nullptr;
	if (BundleAssetList.Num() > 0)
	{
		BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
	}

	TSharedPtr<FStreamableHandle> RawLoadHandle = nullptr;
	//if (RawAssetList.Num() > 0)
	//{
	//	RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(), FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority, TEXT("StartExperienceLoad()"));
	//}

	// If both async loads are running, combine them
	//두 비동기 로드가 모두 실행 중인 경우 이들을 결합합니다
	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}
	else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

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
	TSet<FPrimaryAssetId> PreloadAssetList;
	//@TODO: Determine assets to preload (but not blocking-ly)
	if (PreloadAssetList.Num() > 0)
	{
		AssetManager.ChangeBundleStateForPrimaryAssets(PreloadAssetList.Array(), BundlesToLoad, {});
	}
}

void UPDExperienceManagerComponent::OnExperienceLoadComplete()
{
	check(_LoadState == EPDExperienceLoadState::Loading);
	check(_CurrentExperience != nullptr);

	UE_LOG(LogPDExperience, Log, TEXT("EXPERIENCE: OnExperienceLoadComplete(CurrentExperience = %s, %s)"),
		*_CurrentExperience->GetPrimaryAssetId().ToString(),
		*GetClientServerContextString(this));

	// find the URLs for our GameFeaturePlugins - filtering out dupes and ones that don't have a valid mapping
	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
		{
			for (const FString& PluginName : FeaturePluginList)
			{
				FString PluginURL;
				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, /*out*/ PluginURL))
				{
					This->GameFeaturePluginURLs.AddUnique(PluginURL);
				}
				else
				{
					ensureMsgf(false, TEXT("OnExperienceLoadComplete failed to find plugin URL from PluginName %s for experience %s - fix data, ignoring for this run"), *PluginName, *Context->GetPrimaryAssetId().ToString());
				}
			}

			// 		// Add in our extra plugin
			// 		if (!CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent.IsEmpty())
			// 		{
			// 			FString PluginURL;
			// 			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent, PluginURL))
			// 			{
			// 				GameFeaturePluginURLs.AddUnique(PluginURL);
			// 			}
			// 		}
		};

	CollectGameFeaturePluginURLs(_CurrentExperience, _CurrentExperience->GameFeaturesToEnable);
	for (const TObjectPtr<UPDExperienceActionSet>& ActionSet : _CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			CollectGameFeaturePluginURLs(ActionSet, ActionSet->GameFeaturesToEnable);
		}
	}

	// Load and activate the features	
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading > 0)
	{
		_LoadState = EPDExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			UPDExperienceManager::NotifyOfPluginActivation(PluginURL);
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		OnExperienceFullLoadCompleted();
	}
}

void UPDExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// decrement the number of plugins that are loading
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading == 0)
	{
		OnExperienceFullLoadCompleted();
	}
}

void UPDExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(_LoadState != EPDExperienceLoadState::Loaded);

	// Insert a random delay for testing (if configured)
	if (_LoadState != EPDExperienceLoadState::LoadingChaosTestingDelay)
	{
		const float DelaySecs = PDConsoleVariables::GetExperienceLoadDelayDuration();
		if (DelaySecs > 0.0f)
		{
			FTimerHandle DummyHandle;

			_LoadState = EPDExperienceLoadState::LoadingChaosTestingDelay;
			GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &ThisClass::OnExperienceFullLoadCompleted, DelaySecs, /*bLooping=*/ false);

			return;
		}
	}

	_LoadState = EPDExperienceLoadState::ExecutingActions;

	// Execute the actions
	FGameFeatureActivatingContext Context;

	// Only apply to our specific world context if set
	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				if (Action != nullptr)
				{
					//@TODO: The fact that these don't take a world are potentially problematic in client-server PIE
					// The current behavior matches systems like gameplay tags where loading and registering apply to the entire process,
					// but actually applying the results to actors is restricted to a specific world
					Action->OnGameFeatureRegistering();
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureActivating(Context);
				}
			}
		};

	ActivateListOfActions(_CurrentExperience->Actions);
	for (const TObjectPtr<UPDExperienceActionSet>& ActionSet : _CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	_LoadState = EPDExperienceLoadState::Loaded;

	OnExperienceLoaded_HighPriority.Broadcast(_CurrentExperience);
	OnExperienceLoaded_HighPriority.Clear();

	OnExperienceLoaded.Broadcast(_CurrentExperience);
	OnExperienceLoaded.Clear();

	OnExperienceLoaded_LowPriority.Broadcast(_CurrentExperience);
	OnExperienceLoaded_LowPriority.Clear();

	// Apply any necessary scalability settings
//#if !UE_SERVER
//	UPDSettingsLocal::Get()->OnExperienceLoaded();
//#endif
}

void UPDExperienceManagerComponent::OnActionDeactivationCompleted()
{
	check(IsInGameThread());
	++NumObservedPausers;

	if (NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivated();
	}
}

void UPDExperienceManagerComponent::OnAllActionsDeactivated()
{
	//@TODO: We actually only deactivated and didn't fully unload...
	_LoadState = EPDExperienceLoadState::Unloaded;
	_CurrentExperience = nullptr;
	//@TODO:	GEngine->ForceGarbageCollection(true);
}

void UPDExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnPDExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(_CurrentExperience);
	}
	else
	{
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UPDExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority(FOnPDExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(_CurrentExperience);
	}
	else
	{
		OnExperienceLoaded_LowPriority.Add(MoveTemp(Delegate));
	}
}

void UPDExperienceManagerComponent::SetCurrentExperience(const FPrimaryAssetId& ExperienceId)
{	
	const FSoftObjectPath AssetPath = UPDAssetManager::Get().GetPrimaryAssetPath(ExperienceId);

	TSubclassOf<UPDExperienceDefinition> AssetClass = Cast<UClass>(AssetPath.TryLoad());
	check(AssetClass);
	const UPDExperienceDefinition* Experience = GetDefault<UPDExperienceDefinition>(AssetClass);

	check(Experience != nullptr);
	check(_CurrentExperience == nullptr);
	_CurrentExperience = Experience;
	StartExperienceLoad();
}
