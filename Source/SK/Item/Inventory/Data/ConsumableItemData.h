#pragma once

#include "CoreMinimal.h"
#include "InventoryItemData.h"
#include "ConsumableItemData.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UCLASS()
class SK_API UConsumableItemData : public UInventoryItemData
{
	GENERATED_BODY()

public:
	// 회복량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	int32 Amount;

	// 지속 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	float Duration;

	// 퀵 슬롯 최대 소지량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	int32 QuickSlotSize;

	// 투척 가능 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	bool bIsThrowable;

	// 소모품 GameplayAbility
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	TSubclassOf<UGameplayAbility> ConsumableGA;

	// 소모품 GameplayEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	TSubclassOf<UGameplayEffect> ConsumableGE;
	
};
