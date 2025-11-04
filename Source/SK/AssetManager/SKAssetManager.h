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

	static void DumpLoadedAssets();
	
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
	static void AsynchronousLoadAsset(const FSoftObjectPath& AssetPath, const FStreamableDelegate& OnLoaded);
	
protected:
	
	static bool ShouldLogAssetsLoads();
	
	void AddLoadedAssets(const UObject* Asset);

private:
	TSet<TObjectPtr<const UObject>> LoadedAssets;	

	FCriticalSection LoadedAssetsCritical;
};