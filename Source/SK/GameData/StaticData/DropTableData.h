// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticDataBase.h"
#include "DropTableData.generated.h"

/**
 * 드롭 테이블 데이터 구조
 * 몬스터가 사망 시 참조하는 드롭 정보
 */
USTRUCT(BlueprintType)
struct FDropTableData : public FStaticDataBase
{
	GENERATED_BODY()

public:
	/** 드롭 아이템 ID 배열 (FItemData, FEquipmentData 등 참조 가능) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DropTable")
	TArray<int32> DropItemIDs;

	/** 드롭 확률 (0.0 ~ 1.0, 예: 0.25 → 25%) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DropTable")
	float DropRate = 1.0f;
};