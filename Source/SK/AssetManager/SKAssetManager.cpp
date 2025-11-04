#include "AssetManager/SKAssetManager.h"

// 콘솔 명령 등록
static FAutoConsoleCommand CVarDumpLoadedAssets(
TEXT("SK.DumpLoadedAssets"), // 콘솔에서 입력할 명령어
TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."), // 설명 문자열
	FConsoleCommandDelegate::CreateStatic(USKAssetManager::DumpLoadedAssets) // 콘솔 명령 실행시 호출된 함수 포인터
);

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

void USKAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogTemp, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogTemp, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogTemp, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogTemp, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
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
	return true;
	// 커멘드라인 인자 중 -LogAssetLoads가 포함 되었는지 확인
	// static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	// return bLogAssetLoads;
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
