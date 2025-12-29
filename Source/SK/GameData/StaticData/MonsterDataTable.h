// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticDataBase.h"
#include "MonsterDataTable.generated.h"

/**
 * 몬스터 데이터 테이블 구조
 * 고유 ID, 능력치, 스폰될 클래스, 드롭테이블 연결 등 포함
 */
USTRUCT(BlueprintType)
struct FMonsterData : public FStaticDataBase
{
	GENERATED_BODY()

public:
	/** 몬스터 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	FName MonsterName;

	/** 기본 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	float MaxHealth = 0.f;

	/** 기본 스태미너 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	float MaxStamina = 0.f;

	/** 맥스 밀리 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	int32 MaxMeleeIndex = 0;

	/** 맥스 러쉬 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	int32 MaxRushIndex = 0;
	
	/** 맥스 점프러쉬 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	int32 MaxJumpRushIndex = 0;

	/** 맥스 플라이러쉬 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	int32 MaxFlyRushIndex = 0;
	
	/** 기본 공격력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	float Attack = 0.f;

	/** 기본 방어력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	float Armor = 0.f;

	/** 기본 강인도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	float Poise = 0.f;

	/** 기본 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	float Speed = 0.f;

	/** 백스텝 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	float BackstepDistance = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	TSoftClassPtr<APawn> MonsterClass;

	/** 드롭 테이블 ID (FDropTableData의 ID 참조) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reward")
	int32 DropTableID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reward")
	int32 RewardMinGold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reward")
	int32 RewardMaxGold = 0;

	/** 몬스터 설명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	FText Description;
};