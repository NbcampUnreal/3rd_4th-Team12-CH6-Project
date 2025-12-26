// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameData/SpawnPointData.h"
#include "SpawnPointActor.generated.h"

UCLASS()
class SK_API ASpawnPointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnPointActor();
	

	// 기존 SpawnPointData에 들어갈 값들만 노출
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	int32 SpawnRuleID = 0;

	// 방/구역 단위로 리스폰하려면 그룹이 필요
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	int32 GroupID = 0;
	
	// 이 포인트에서 허용되는 최대 마릿수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	int32 MaxSpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	bool bSpawnOnBeginPlay = true;

	//핵심: 기존 SpawnPointData를 만들어주는 함수
	void BuildSpawnPointData(FSpawnPointData& OutData) const;

protected:
	virtual void BeginPlay() override;

};
