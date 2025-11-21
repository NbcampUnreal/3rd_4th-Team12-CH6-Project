// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticDataBase.h"
#include "NPCDataTable.generated.h"

/**
 * NPC 데이터 테이블 구조
 * NPC 이름, 클래스, 대화 ID 포함
 */
USTRUCT(BlueprintType)
struct FNPCData : public FStaticDataBase
{
	GENERATED_BODY()

public:
	/** NPC 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NPC")
	FName NPCName;

	/** NPC 액터 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NPC")
	TSoftClassPtr<AActor> NPCClass;

	/** 연결된 대화 데이터 ID (FDialogueData 참조) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NPC")
	int32 DialogueID = -1;
};