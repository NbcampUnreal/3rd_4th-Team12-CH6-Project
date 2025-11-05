// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/StaticDataSubsystem.h"
#include "GameData/StaticData/StaticDataBase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "GameData/StaticData/ItemDataTable.h"

void UStaticDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("[StaticDataSubsystem] Initialize() - Auto Loading DataTables"));
	AutoRegisterDataTables();
}

void UStaticDataSubsystem::Deinitialize()
{
	for (auto& Pair : DataManagers)
	{
		if (Pair.Value)
			Pair.Value->Unload();
	}
	DataManagers.Empty();

	UE_LOG(LogTemp, Log, TEXT("[StaticDataSubsystem] Deinitialize() - Data cleared"));
	Super::Deinitialize();
}

void UStaticDataSubsystem::AutoRegisterDataTables()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> FoundAssets;
	AssetRegistryModule.Get().GetAssetsByPath(FName("/Game/BluePrint/GameData/StaticData/"), FoundAssets, true);

	for (const FAssetData& Asset : FoundAssets)
	{
		UDataTable* DataTable = Cast<UDataTable>(Asset.GetAsset());
		if (!DataTable) return;
		
		const UScriptStruct* RowStruct = DataTable->GetRowStruct();
		if (!RowStruct)
			continue;

		// FStaticDataBase를 상속한 구조체만 자동 등록
		if (!RowStruct->IsChildOf(FStaticDataBase::StaticStruct()))
		{
			UE_LOG(LogTemp, Warning, TEXT("[StaticDataSubsystem] DataTable %s skipped (RowStruct not derived from FStaticDataBase)"), *Asset.AssetName.ToString());
			continue;
		}

		TSharedPtr<FDataManagerBase> Manager = MakeShared<TStaticDataManager<FStaticDataBase>>();
		Manager->Unload(); // 캐시 초기화
		DataManagers.Add(RowStruct->GetName(), Manager);
			
		auto TypedManager = MakeShared<TStaticDataManager<FStaticDataBase>>();
		TypedManager->Initialize(DataTable);
		DataManagers[RowStruct->GetName()] = TypedManager;

		UE_LOG(LogTemp, Log, TEXT("[StaticDataSubsystem]  Auto-Registered DataTable: %s (Struct: %s)"),
			*DataTable->GetName(), *RowStruct->GetName());
		
	}

	UE_LOG(LogTemp, Log, TEXT("[StaticDataSubsystem] Initialization Complete (%d types)"), DataManagers.Num());
}