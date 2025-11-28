// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/StaticData/StaticDataBase.h"
#include "MonsterSpawnRule.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMonsterSpawnRule : public FStaticDataBase
{
	GENERATED_BODY();

public:

	// PCG의 SpawnRuleID와 연결
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnRuleID = -1;

	// 등장 가능한 MonsterID들
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int32> MonsterIDs;

	// 랜덤 가중치 (MonsterIDs와 인덱스 1:1 대응)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int32> SpawnWeights;

	// 최소~최대 스폰 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int32> MinCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int32> MaxCount;

	//설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
};