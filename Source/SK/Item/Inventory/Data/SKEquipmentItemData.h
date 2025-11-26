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

UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	None UMETA(DisplayName = "None"),

	// 무기
	Weapon UMETA(DisplayName = "Weapon"),

	// 방어구
	Helmet UMETA(DisplayName = "Helmet"),
	Chest UMETA(DisplayName = "Chest"),
	Leg UMETA(DisplayName = "Leg"),
	Boots UMETA(DisplayName = "Boots"),

	// 장신구
	Accessory1 UMETA(DisplayName = "Accessory 1"),
	Accessory2 UMETA(DisplayName = "Accessory 2")
};

UENUM(BlueprintType)
enum class EEquipmentStat : uint8
{
	Attack,
	Defense,
	Stamina,
	Health,
	// 필요에 따라 계속 추가
};

USTRUCT(BlueprintType)
struct FEquipData
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
class SK_API USKEquipmentItemData : public USKInventoryItemData
{
	GENERATED_BODY()

public:
	USKEquipmentItemData()
	{
		InventoryType = EInventoryItemType::Equipment;
	}
	
	// 장비 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	EEquipmentItemType EquipmentType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	EEquipmentSlotType SlotType;
	
	// 착용 슬롯
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	int32 AttachmentSlot;

	// 장비 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	// 장착 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	FName SocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	TMap<EEquipmentStat, FEquipData> Stats;
	
	// 장비 GameplayAbility
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	TSubclassOf<UGameplayAbility> EquipmentGA;

	// 장비 GameplayEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Equipment")
	TSubclassOf<UGameplayEffect> EquipmentGE;
};
