// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameData/StaticData/StaticDataManager.h"
#include "StaticDataSubsystem.generated.h"

/**
 * 정적 게임 데이터 서브시스템
 * - 모든 DataTable을 자동으로 로드하고
 * - TStaticDataManager<T> 형태로 관리
 * - 템플릿 기반으로 데이터 접근 제공
 */
UCLASS()
class SK_API UStaticDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** 특정 타입(TStruct)의 매니저 가져오기 */
	template<typename TStruct>
	TStaticDataManager<TStruct>* GetManager()
	{
		const FString TypeName = TStruct::StaticStruct()->GetName();
		TSharedPtr<FDataManagerBase>* Found = DataManagers.Find(TypeName);
		if (!Found)
		{
			UE_LOG(LogTemp, Warning, TEXT("[StaticDataSubsystem] Manager not found for type: %s"), *TypeName);
			return nullptr;
		}
		
		return static_cast<TStaticDataManager<TStruct>*>(Found->Get());
	}

	/** 특정 타입의 데이터 가져오기 (ID 기반) */
	template<typename TStruct>
	const TStruct* GetData(int32 ID)
	{
		auto* Manager = GetManager<TStruct>();
		if (!Manager) return nullptr;

		const TStruct* Data = Manager->Get(ID);
		if (!Data)
		{
			UE_LOG(LogTemp, Warning, TEXT("[StaticDataSubsystem] GetData<%s>(%d) failed — ID not found in table."),
				*FString(TStruct::StaticStruct()->GetName()), ID);
			return nullptr;
		}
		
		return Data;
	}

	/** 특정 타입(TStruct)의 모든 데이터를 반환 */
	template<typename TStruct>
	TArray<const TStruct*> GetAllData()
	{
		TArray<const TStruct*> OutArray;

		auto* Manager = GetManager<TStruct>();
		if (!Manager) return OutArray;
		
		Manager->GetAll(OutArray);

		if (OutArray.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("[StaticDataSubsystem] GetAllData<%s>() — No rows found in table."),
				*FString(TStruct::StaticStruct()->GetName()));
		}

		return OutArray;
	}

public:
	/** 데이터 테이블 세팅 */
	UFUNCTION(BlueprintCallable)
	void AutoRegisterDataTables();

private:
	TMap<FString, TSharedPtr<FDataManagerBase>> DataManagers;
};
