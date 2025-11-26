#pragma once

#include "CoreMinimal.h"
#include "SKInventoryItemData.h"
#include "Utility/SKNativeGameplayTags.h"
#include "SKConsumableItemData.generated.h"

class UGameplayEffect;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FUseItemData
{
	GENERATED_BODY()
 
	// SetByCaller용 Tag (예: Data.AttackPower, Data.BuffDuration 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag DataTag;
 
	// 1.0f, 10.f 등 Magnitude 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Magnitude = 0.f;
};


UCLASS()
class SK_API USKConsumableItemData : public USKInventoryItemData
{
	GENERATED_BODY()

public:
	USKConsumableItemData()
	{
		InventoryType = EInventoryItemType::Consumable;
	}

	/**
	* 아이템 사용 시 적용될 SetByCaller 파라미터 목록입니다.
	* 각 Param에는 적용할 GameplayTag(ParamTag)와 그에 대응하는 Magnitude 값을 설정합니다.
	*
	* ※ 지속 시간이 있는 소비형 아이템의 경우
	* 반드시 Duration 관련 태그(예: TAG_Item_Consume_Data_Duration)와
	* 해당 지속 시간을 나타내는 Magnitude 값을 포함해야 합니다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Consumable")
	TArray<FUseItemData> Params;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Consumable")
	float Cooldown = 3.f;
	
	// 퀵 슬롯 최대 소지량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	int32 QuickSlotSize;

	
	/**
	* 소모품 GameplayAbility
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	TSubclassOf<UGameplayAbility> ConsumableGA;

	/**
	* 소모품 GameplayEffect
	* 값이 올라가고 끝이나는 경우 ConsumableSinge
	* 지속시간이 있는 경우 ConsumableDuration을 기반으로 한 Effect를 선택합니다.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Consumable")
	TSubclassOf<UGameplayEffect> ConsumableGE;
};
