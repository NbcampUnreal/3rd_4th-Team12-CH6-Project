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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Misc")
	bool bIsQuestItem;

	// 통화 아이템 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Misc")
	bool bIsCurrency;

	// 열쇠 아이템 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Misc")
	bool bIsKeyItem;

	// 제작 재료 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Misc")
	bool bIsMaterial;
};
