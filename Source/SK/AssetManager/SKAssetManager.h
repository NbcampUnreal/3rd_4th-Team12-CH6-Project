#pragma once

#include "Engine/AssetManager.h"
#include "SKAssetManager.generated.h"

UCLASS()
class SK_API USKAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	USKAssetManager();

	static USKAssetManager& Get();

	virtual void StartInitialLoading() override;

protected:

	// 동기 로드
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	// 비동기 로드
	static void AsynchronousLoadAsset(const FSoftObjectPath& AssetPath, const FStreamableDelegate& OnLoaded);
	
	static bool ShouldLogAssetsLoads();

	void AddLoadedAssets(const UObject* Asset);


private:
	TSet<TObjectPtr<const UObject>> LoadedAssets;	

	FCriticalSection LoadedAssetsCritical;
};
