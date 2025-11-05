// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticDataBase.h"
#include "GameplayTagContainer.h"
#include "DialogueDataTable.generated.h"

/**
 * 대화(Dialogue) 데이터 구조
 * NPC가 사용하는 대사 및 이벤트 트리거
 */
USTRUCT(BlueprintType)
struct FDialogueData : public FStaticDataBase
{
	GENERATED_BODY()

public:
	/** 대사 내용 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue")
	FText DialogueLine;

	/** 다음 대화의 ID (연속 대화용) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue")
	int32 NextDialogueID = -1;

	/** 특정 이벤트를 트리거하는 GameplayTag (예: 퀘스트 시작, 상호작용 등) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue")
	FGameplayTag EventTag;
};