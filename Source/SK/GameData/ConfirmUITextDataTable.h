// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "ConfirmUITextDataTable.generated.h"

/**
 * 확인창에서 표시할 텍스트 데이터 구조
 */
USTRUCT(BlueprintType)
struct FConfirmUITextRow : public FTableRowBase
 {
 	GENERATED_BODY()

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConfirmUI")
 	FText Title;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConfirmUI")
 	FText Message;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConfirmUI")
 	FText ConfirmText;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConfirmUI")
 	FText CancelText;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConfirmUI")
 	FGameplayTag SlotTag;
 };
