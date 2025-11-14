// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/ObjectPoolingSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "GameData/ObjectPoolDataAsset.h"
#include "UObject/TopLevelAssetPath.h"

void UObjectPoolingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("[ObjectPoolingSubsystem] Initialize() called."));

    /*
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> FoundAssets;
    FTopLevelAssetPath AssetClassPath(UObjectPoolDataAsset::StaticClass());
    AssetRegistryModule.Get().GetAssetsByClass(AssetClassPath, FoundAssets);

    if (FoundAssets.Num() > 0)
    {
        DefaultConfigAsset = Cast<UObjectPoolDataAsset>(FoundAssets[0].GetAsset());
        UE_LOG(LogTemp, Warning, TEXT("[ObjectPoolingSubsystem] Auto-detected DataAsset: %s"), *FoundAssets[0].AssetName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[ObjectPoolingSubsystem] No UObjectPoolDataAsset found in content folder."));
    }

    if (DefaultConfigAsset)
    {
        UE_LOG(LogTemp, Log, TEXT("[ObjectPoolingSubsystem] Pool initialized from asset: %s"), *DefaultConfigAsset->GetName());
        InitializeFromDataAsset(DefaultConfigAsset);
    }
    */
}

void UObjectPoolingSubsystem::InitializeFromDataAsset(UObjectPoolDataAsset* ConfigAsset)
{
    UWorld* World = GetWorld();
    if (!ConfigAsset || !World)
    {
        UE_LOG(LogTemp, Log, TEXT("[ObjectPoolingSubsystem] null"));
        return;
    }

    // 🔹 컴포넌트 풀용 루트 액터 생성
    if (!ComponentRootActor)
    {
        ComponentRootActor = World->SpawnActor<AActor>();
        ComponentRootActor->SetActorHiddenInGame(true);
        ComponentRootActor->SetActorEnableCollision(false);
    }
    
    for (const FActorPoolConfig& Config : ConfigAsset->ActorPools)
    {
        // Actor Pool 초기화
        if (Config.ActorClass.IsValid())
        {
            ActorPool.InitializePool(
                World,
                Config.PreloadCount, 
                [Class = Config.ActorClass](UWorld* W)
                {
                    return W->SpawnActor<AActor>(Class.LoadSynchronous(), FVector::ZeroVector, FRotator::ZeroRotator);
                },
                PoolActionDefine::Actor::Deactivate
            );
            UE_LOG(LogTemp, Warning, TEXT("[ObjectPoolingSubsystem] pools from DataAsset: %s"), *Config.ActorClass->GetName());
        }
    }

    // Sound Pool 초기화
    for (const FSoundPoolConfig& Config : ConfigAsset->SoundPools)
    {
        if (Config.SoundAsset.IsValid())
        {
            SoundPool.InitializePool(
                World,
                Config.PreloadCount,
                [this, Sound = Config.SoundAsset](UWorld* W)
                {
                    UAudioComponent* Comp = NewObject<UAudioComponent>(ComponentRootActor);
                    Comp->SetSound(Sound.Get());
                    Comp->bAutoActivate = false;
                    Comp->RegisterComponent();
                    return Comp;
                },
                PoolActionDefine::Audio::Deactivate
            );
            UE_LOG(LogTemp, Warning, TEXT("[ObjectPoolingSubsystem] pools from DataAsset: %s"), *Config.SoundAsset->GetName());
        }
    }

    // Niagara Pool 초기화
    for (const FNiagaraPoolConfig& Config : ConfigAsset->NiagaraPools)
    {
        if (Config.NiagaraAsset.IsValid())
        {
            NiagaraPool.InitializePool(
                World,
                Config.PreloadCount,
                [this, System = Config.NiagaraAsset](UWorld* W)
                {
                    //UNiagaraSystem* Niagara = System.LoadSynchronous();
                    //if (!Niagara) return (UNiagaraComponent*)nullptr;

                    // 🔹 미리 스폰해서 풀에 등록
                    UNiagaraComponent* Comp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                        W,
                        System.Get(),
                        FVector::ZeroVector,
                        FRotator::ZeroRotator,
                        FVector(1.f),
                        false, // AutoDestroy = false
                        false, // AutoActivate = false
                        ENCPoolMethod::None);

                    Comp->DeactivateImmediate();
                    Comp->SetVisibility(false);
                    Comp->SetAutoActivate(false);
                    Comp->SetActive(false);
                    return Comp;
                },
                PoolActionDefine::Niagara::Deactivate
            );
            UE_LOG(LogTemp, Warning, TEXT("[ObjectPoolingSubsystem] pools from DataAsset: %s"), *Config.NiagaraAsset->GetName());
        }
    }
}


void UObjectPoolingSubsystem::Deinitialize()
{
    ActorPool.ClearAllPools();
    SoundPool.ClearAllPools();
    NiagaraPool.ClearAllPools();

    // Root Actor 제거
    if (IsValid(ComponentRootActor))
    {
        ComponentRootActor->Destroy();
        ComponentRootActor = nullptr;
    }
    
    UE_LOG(LogTemp, Log, TEXT("[UObjectPoolingSubsystem] All Pools Cleared"));
    Super::Deinitialize();
}
