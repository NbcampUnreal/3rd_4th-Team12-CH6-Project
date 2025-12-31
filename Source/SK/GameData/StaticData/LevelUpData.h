// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticDataBase.h"
#include "LevelUpData.generated.h"

/**
 * 레벨업 테이블 데이터 구조
 */
USTRUCT(BlueprintType)
struct FLevelUpData : public FStaticDataBase
{
	GENERATED_BODY()

public:
	// 레벨업에 필요한 Gold
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RequiredGold = 0;
	
	UPROPERTY(EditAnywhere)
	float MaxHP;

	UPROPERTY(EditAnywhere)
	float Attack;

	UPROPERTY(EditAnywhere)
	float Armor;

	// 지급할 AbilityPoint
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AbilityPointReward = 1;
};