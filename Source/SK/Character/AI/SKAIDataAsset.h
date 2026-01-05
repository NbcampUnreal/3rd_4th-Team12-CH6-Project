#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "SKAIDataAsset.generated.h"

class ASKBaseProjectile;
class UStateTree;

UCLASS()
class SK_API USKAIDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Health")
	float Health = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Health")
	float MaxHealth = 100.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float Attack = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float Armor = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float Poise = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Speed")
	float Speed = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Speed")
	float SprintWeight = 1.5f;

	*/

	// 드랍골드, 드랍아이템, 드랍경험치 등 몬스터데이터테이블 연동?	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FGameplayTag TeamTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FGameplayTag TypeTag;
	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FGameplayTagContainer AbilityTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FGameplayTagContainer InitialStateTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material")
	UMaterialInstance* MaterialInstance;
	*/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> StartupGE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag|Effect")
	TSubclassOf<UGameplayEffect> GiveTeamTagEffect;
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TMap<FName, TObjectPtr<UAnimMontage>> Montages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TMap<FName, TSubclassOf<ASKBaseProjectile>> ProjectileClasses;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTree> StateTreeAsset;
};
