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

	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

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

template <typename AssetType>
AssetType* USKAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAssets(Cast<UObject>(LoadedAsset));
		}
	}
	
	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> USKAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			Get().AddLoadedAssets(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}