// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticDataBase.h"
#include "Item/Pickup/Data/SKPickupItemData.h"
#include "DropTableData.generated.h"

/**
 * 개별 드랍 아이템 정보
 */
USTRUCT(BlueprintType)
struct FDropItemData
{
	GENERATED_BODY();

	// 드랍 확률 (0~1)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Probability = 1.0f;

	// 최소/최대 드랍 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount = 1;

	// 어떤 아이템을 드랍할지 (ItemID는 PickupData 내부에 존재)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USKPickupItemData> PickupData;
};

/**
 * 드롭 테이블 데이터 구조
 * 몬스터가 사망 시 참조하는 드롭 정보
 */
USTRUCT(BlueprintType)
struct FDropTableData : public FStaticDataBase
{
	GENERATED_BODY()

public:
	// 드랍 항목 배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDropItemData> Items;
};