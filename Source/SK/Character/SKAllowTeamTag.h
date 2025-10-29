// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "SKAllowTeamTag.generated.h"

USTRUCT(BlueprintType)
struct FSKAllowTeamTag : public FTableRowBase
{
	GENERATED_BODY()

public:
	//아이템이나 플레이어 팀태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Tag")
	FGameplayTag AssignedTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Tag")
	FGameplayTagContainer		AllowedTeamTag;

	// 스폰할 아이템 이나 드롭 아이템
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly , Category="SK|Item")
	TArray<TSubclassOf<AActor>> ItemClasses;

	// 스폰할 아이템 주기
	UPROPERTY(EditAnywhere, BlueprintReadOnly , Category="SK|Item")
	float SpawnPeriodSec;
	
	
};