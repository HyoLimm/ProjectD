// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/System/PDAssetManager.h"
#include "Game/PDLogChannels.h"

const FName FPDBundles::Equipped("Equipped");


UPDAssetManager::UPDAssetManager()
{
	DefaultPawnData = nullptr;
}

UPDAssetManager& UPDAssetManager::Get()
{
	check(GEngine);

	if (UPDAssetManager* Singleton = Cast<UPDAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogPD, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to PDAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UPDAssetManager>();
}

const UPDPawnData* UPDAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

UObject* UPDAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool UPDAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void UPDAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}
