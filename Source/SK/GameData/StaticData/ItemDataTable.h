// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticDataBase.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "ItemDataTable.generated.h"

/**
 * 아이템 데이터 테이블 구조
 * 아이템 ID, 이름, 관련 클래스 정보 포함
 */
USTRUCT(BlueprintType)
struct FItemData : public FStaticDataBase
{
	GENERATED_BODY()

public:
	/** 아이템 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TSoftObjectPtr<USKInventoryItemData> InventoryItemDataAsset;
	
	/** 아이템 설명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FText Description;
	
};