#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

/**
 * 모든 매니저의 공통 인터페이스 (폴리모픽 접근 가능)
 */
class FDataManagerBase
{
public:
	virtual ~FDataManagerBase() = default;
	virtual void Unload() = 0;
};

/**
 * 특정 Row 타입(TStruct)에 대한 데이터 매니저
 * - DataTable 로드 및 캐싱 관리
 * - ID 기반 빠른 조회 제공
 */
template<typename TStruct>
class TStaticDataManager : public FDataManagerBase
{
public:
	TStaticDataManager() = default;
	virtual ~TStaticDataManager() override { Unload(); }

	/** DataTable 로드 후 모든 Row 캐싱 */
	void Initialize(UDataTable* InDataTable)
	{
		if (!InDataTable) return;
		
		DataTable = InDataTable;
		CachedData.Empty();

		TArray<TStruct*> Rows;
		InDataTable->GetAllRows(TEXT("StaticDataManager"), Rows);

		for (TStruct* Row : Rows)
		{
			if (Row)
			{
				CachedData.Add(Row->ID, Row);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded %d entries for %s"),
			CachedData.Num(),
			*InDataTable->GetName());
	}

	/** 특정 ID의 Row를 가져오기 */
	const TStruct* Get(int32 ID) const
	{
		if (const TStruct* const* Found = CachedData.Find(ID))
			return *Found;
		return nullptr;
	}

	/** 모든 Row를 배열로 반환 */
	void GetAll(TArray<const TStruct*>& OutArray) const
	{
		for (auto& Pair : CachedData)
			OutArray.Add(Pair.Value);
	}

	/** 매니저 언로드 (캐시 해제) */
	virtual void Unload() override
	{
		CachedData.Empty();
		DataTable = nullptr;
	}

private:
	UDataTable* DataTable = nullptr;
	TMap<int32, const TStruct*> CachedData;
};
