#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SKWeaponActionData.generated.h"

class USKPlayerAnimInstance;

UCLASS()
class SK_API USKWeaponActionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon")
	FName WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|AnimClass")
	TSubclassOf<USKPlayerAnimInstance> AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon")
	FGameplayTag WeaponTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Hit")
	TObjectPtr<UAnimMontage> HitMontage;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Hit")
	TObjectPtr<UAnimMontage> KnockBackMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Die")
	TArray<TObjectPtr<UAnimMontage>> DieMontages;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Dodge|Evade")
	TObjectPtr<UAnimMontage> EvadeMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Dodge|Step")
	TObjectPtr<UAnimMontage> StepMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Block")
	TObjectPtr<UAnimMontage> BlockMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Gaurd")
	TObjectPtr<UAnimMontage> GuardMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Getup")
	TObjectPtr<UAnimMontage> GetupMontage;
};
