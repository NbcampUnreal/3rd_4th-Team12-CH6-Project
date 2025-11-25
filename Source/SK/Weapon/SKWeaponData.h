#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKWeaponData.generated.h"

class USKWeaponData;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None UMETA(DisplayName = "None"),
	TwoHanded UMETA(DisplayName = "TwoHanded"),
	Dual UMETA(DisplayName = "Dual"),
	GreatSword UMETA(DisplayName = "GreatSword"),
	Katana UMETA(DisplayName = "Katana"),
	Spear UMETA(DisplayName = "Spear"),
	Warrior UMETA(DisplayName = "Warrior")
};

USTRUCT(BlueprintType)
struct FWeaponDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponType WeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USKWeaponData> WeaponData;
};


class UGameplayEffect;
class UAnimMontage;

UCLASS()
class SK_API USKWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon")
	FName WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon")
	EWeaponType WeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Attack")
	TArray<TObjectPtr<UAnimMontage>> LeftAttackMontages;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Attack")
	TArray<TObjectPtr<UAnimMontage>> RightAttackMontages;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Skill")
	TArray<TObjectPtr<UAnimMontage>> SkillMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Equip")
	TObjectPtr<UAnimMontage> EquipMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Equip")
	TObjectPtr<UAnimMontage> UnEquipMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|GameplayEffect")
	TSubclassOf<UGameplayEffect> WeaponGameplayEffect;
	
};
