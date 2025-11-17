#pragma once

#include "CoreMinimal.h"
#include "InventoryItemData.h"
#include "MiscItemData.generated.h"

UCLASS()
class SK_API UMiscItemData : public UInventoryItemData
{
	GENERATED_BODY()

public:
	// 퀘스트 아이템 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	bool bIsQuestItem;

	// 통화 아이템 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	bool bIsCurrency;

	// 열쇠 아이템 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	bool bIsKeyItem;

	// 제작 재료 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	bool bIsMaterial;
};
