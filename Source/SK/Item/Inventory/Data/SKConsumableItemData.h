#pragma once

#include "CoreMinimal.h"
#include "SKInventoryItemData.h"
#include "SKConsumableItemData.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UCLASS()
class SK_API USKConsumableItemData : public USKInventoryItemData
{
	GENERATED_BODY()

public:
	USKConsumableItemData()
	{
		InventoryType = EInventoryItemType::Consumable;
	}
	// 회복량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	int32 Amount;

	// 지속 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	float CoolDown;

	// 퀵 슬롯 최대 소지량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	int32 QuickSlotSize;

	// 소모품 GameplayAbility
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	TSubclassOf<UGameplayAbility> ConsumableGA;
};
