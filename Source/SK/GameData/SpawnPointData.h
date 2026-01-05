// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnPointData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSpawnPointData
{
	GENERATED_BODY();

	// PCG가 지정한 스폰 좌표 (대략적인 위치)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	// 어떤 스폰 규칙을 적용할지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnRuleID = -1;

	// 방/구역 단위로 리스폰하려면 그룹이 필요
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GroupID = 0;

	// 리스폰/웨이브를 위해 저장하는 스폰된 액터 목록
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> SpawnedActors;
	
};