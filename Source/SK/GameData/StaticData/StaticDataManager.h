#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

/**
 * 모든 매니저의 공통 인터페이스
 */
class FDataManagerBase
{
public:
	virtual ~FDataManagerBase() = default;
	virtual void Unload() = 0;
};

/**
 * 특정 Row 타입(TStruct)에 대한 데이터 매니저
 * - DataTable Soft 참조
 * - 필요 시 로드 후 즉시 조회
 * - Row 캐싱 없음 (GC-safe)
 */
template<typename TStruct>
class TStaticDataManager : public FDataManagerBase
{
public:
	TStaticDataManager() = default;
	virtual ~TStaticDataManager() override { Unload(); }

	/** DataTable Soft Reference 설정 */
	void Initialize(const TSoftObjectPtr<UDataTable>& InDataTable)
	{
		DataTable = InDataTable;
	}

	/** 특정 ID의 Row 조회 (즉시 사용용) */
	const TStruct* Get(int32 ID) const
	{
		UDataTable* Table = GetTable();
		if (!Table) return nullptr;

		TArray<TStruct*> Rows;
		Table->GetAllRows(TEXT("StaticDataManager::Get"), Rows);

		for (const TStruct* Row : Rows)
		{
			if (Row && Row->ID == ID)
			{
				return Row; // 즉시 사용 (보관 금지)
			}
		}
		return nullptr;
	}

	/** 모든 Row 순회 (콜백 방식 권장) */
	template<typename FuncType>
	void ForEach(FuncType Func) const
	{
		UDataTable* Table = GetTable();
		if (!Table) return;

		TArray<TStruct*> Rows;
		Table->GetAllRows(TEXT("StaticDataManager::ForEach"), Rows);

		for (TStruct* Row : Rows)
		{
			if (Row)
			{
				Func(*Row);
			}
		}
	}

	/** 언로드 */
	virtual void Unload() override
	{
		DataTable.Reset();
	}

private:
	/** Soft 로딩 헬퍼 */
	UDataTable* GetTable() const
	{
		if (!DataTable.IsValid())
		{
			return DataTable.LoadSynchronous();
		}
		return DataTable.Get();
	}

private:
	TSoftObjectPtr<UDataTable> DataTable;
};
