#pragma once

#include "CoreMinimal.h"
#include "InventoryItemData.h"
#include "EquipmentItemData.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UENUM(BlueprintType)
enum class EEquipmentItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
	Accessory UMETA(DisplayName = "Accessory")
};

UCLASS()
class SK_API UEquipmentItemData : public UInventoryItemData
{
	GENERATED_BODY()

public:
	// 장비 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	EEquipmentItemType EquipmentType;

	// 착용 슬롯
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	int32 AttachmentSlot;

	// 장비 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	// 장착 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	FName SocketName;

	// 공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	float AttackPower;

	// 방어력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	float DefensePower;

	// 장비 GameplayAbility
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	TSubclassOf<UGameplayAbility> EquipmentGA;

	// 장비 GameplayEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Inventory")
	TSubclassOf<UGameplayEffect> EquipmentGE;
	
};
