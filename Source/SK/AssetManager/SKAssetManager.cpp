#include "AssetManager/SKAssetManager.h"

USKAssetManager::USKAssetManager()
{
}

USKAssetManager& USKAssetManager::Get()
{
	USKAssetManager* This = Cast<USKAssetManager>(GEngine->AssetManager);
	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("InValid AssetManagr in DefaultEngine.ini, must be SKAssetManger!"))
		return *NewObject<USKAssetManager>(); // never calls this
	}
}

void USKAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}

UObject* USKAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		// 디버깅 로그 제어
		if (ShouldLogAssetsLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously load asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}
		// 에셋 매니저 활성화 확인
		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		return AssetPath.TryLoad();
	}
	return nullptr;
}

void USKAssetManager::AsynchronousLoadAsset(const FSoftObjectPath& AssetPath, const FStreamableDelegate& OnLoaded)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;
		
		// 디버깅 로그 제어
		if (ShouldLogAssetsLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Asynchronously load asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}
		// 에셋 매니저 활성화 시 비동기 로드
		if (UAssetManager::IsInitialized())
		{
			UAssetManager::GetStreamableManager().RequestAsyncLoad(AssetPath, OnLoaded, FStreamableManager::AsyncLoadHighPriority);
		}
		// 비활성화 시 동기 로드로 대체
		else
		{
			UObject* Loaded = AssetPath.TryLoad();
			if (Loaded)
			{
				OnLoaded.ExecuteIfBound();
			}
		}
	}
}

bool USKAssetManager::ShouldLogAssetsLoads()
{
	// 커멘드라인 인자 중 -LogAssetLoads가 포함 되었는지 확인
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void USKAssetManager::AddLoadedAssets(const UObject* Asset)
{
	// 포인터가 nullptr인지 런타임 검사
	if (ensureAlways(Asset))
	{
		// 멀티 스레드 동시 접근 방지용 락
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}
