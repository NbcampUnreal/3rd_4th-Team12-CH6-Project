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
	float MaxHP = 0.f;

	/** 기본 공격력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	float Attack = 0.f;

	/** 기본 방어력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	float Defence = 0.f;

	/** 스폰할 액터 클래스 (예: 몬스터 블루프린트) */
	//APawn? AActor?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	TSoftClassPtr<APawn> MonsterClass;
	//TSoftClassPtr<AActor> MonsterClass;

	/** 드롭 테이블 ID (FDropTableData의 ID 참조) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	int32 DropTableID = -1;

	/** 몬스터 설명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Monster")
	FText Description;
};