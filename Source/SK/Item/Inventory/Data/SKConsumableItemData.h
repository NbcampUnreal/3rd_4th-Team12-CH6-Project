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
	// 회복량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	int32 Amount;

	// 지속 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	float Duration;

	// 퀵 슬롯 최대 소지량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	int32 QuickSlotSize;

	// 투척 가능 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	bool bIsThrowable;

	// 소모품 GameplayAbility
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	TSubclassOf<UGameplayAbility> ConsumableGA;

	// 소모품 GameplayEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	TSubclassOf<UGameplayEffect> ConsumableGE;
	
};
