#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StaticDataBase.generated.h"

/**
 * 모든 정적 데이터(Row)의 공통 부모
 * - 반드시 DataTable RowStruct는 이 타입을 상속해야 함
 * - ID 필드를 공통으로 제공 (자동 등록 및 조회의 Key로 사용됨)
 */
USTRUCT(BlueprintType)
struct FStaticDataBase : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StaticData")
	int32 ID = -1;
};