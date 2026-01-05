// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/StaticData/StaticDataBase.h"
#include "MonsterSpawnRule.generated.h"

/**
 *  몬스터 스폰 구조체
 * 
 */
USTRUCT(BlueprintType)
struct FMonsterSpawnEntry
{
	GENERATED_BODY()

	// 어떤 몬스터인가
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MonsterID = -1;

	// 이 몬스터의 스폰 최소 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinCount = 1;

	// 이 몬스터의 스폰 최대 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount = 1;
};

/**
 * 실제
 */
USTRUCT(BlueprintType)
struct FMonsterSpawnRule : public FStaticDataBase
{
	GENERATED_BODY();

public:
	
	// 몬스터 스폰 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMonsterSpawnEntry> SpawnEntries;
	
	//설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
};