#pragma once

#include "CoreMinimal.h"
#include "SKInventoryItemData.h"
#include "SKEquipmentItemData.generated.h"

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
class SK_API USKEquipmentItemData : public USKInventoryItemData
{
	GENERATED_BODY()

public:
	// 장비 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	EEquipmentItemType EquipmentType;

	// 착용 슬롯
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	int32 AttachmentSlot;

	// 장비 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	// 장착 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	FName SocketName;

	// 공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	float AttackPower;

	// 방어력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	float DefensePower;

	// 장비 GameplayAbility
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	TSubclassOf<UGameplayAbility> EquipmentGA;

	// 장비 GameplayEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	TSubclassOf<UGameplayEffect> EquipmentGE;
	
};
