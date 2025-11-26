#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SKWeaponData.generated.h"

class USKWeaponStateData;
class USKWeaponData;

USTRUCT(BlueprintType)
struct FWeaponDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag WeaponTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USKWeaponData> WeaponData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USKWeaponStateData> WeaponStateData;
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
	FGameplayTag WeaponTag;
	
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
