#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SKInventoryItemData.generated.h"

UENUM(BlueprintType)
enum class EInventoryItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Equipment UMETA(DisplayName = "Equipment"),
	Consumable UMETA(DisplayName = "Consumable"),
	Misc UMETA(DisplayName = "Misc")
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	None UMETA(DisplayName = "None"),
	Common UMETA(DisplayName = "Common"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")
};

UCLASS()
class SK_API USKInventoryItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 아이템 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	FName ItemID;

	// 아이템 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	FText ItemName;

	// 아이템 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	FText ItemDescription;

	// 인벤토리 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	EInventoryItemType InventoryType;

	// 아이템 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	FGameplayTag ItemTag;

	// 아이템 희귀도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	EItemRarity ItemRarity;

	// 스택 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	bool bIsStackable;

	// 인벤토리 슬롯 최대 사이즈
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	bool SlotSize;
	
	// UI 표시용 아이템 아이콘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	TObjectPtr<UTexture2D> ItemIcon;

	// UI 표시용 텍스트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	FText ItemText;
};
