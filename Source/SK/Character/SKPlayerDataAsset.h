// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"
#include "InputTriggers.h"
#include "SKPlayerDataAsset.generated.h"


USTRUCT(BlueprintType)
struct FInputActionTagMapping
{
	GENERATED_BODY()

	FInputActionTagMapping()
		: InputAction(nullptr)
		, Tag(FGameplayTag::EmptyTag)
		, TriggerEvent(ETriggerEvent::Started)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETriggerEvent TriggerEvent;
};


/**
 * 
 */
UCLASS()
class SK_API USKPlayerDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
#pragma region AttributeSet
	//  체력 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Speed")
	float Speed = 700.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Speed")
	float SprintWeight = 1.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Health")
	float Health = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Health")
	float MaxHealth = 100.f;

	//  스태미나 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Stamina")
	float Stamina = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Stamina")
	float MaxStamina = 50.f;

	//  히트(Heat) 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Heat")
	float Heat = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Heat")
	float MaxHeat = 100.f;

	// 경험치 & 레벨 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Progression")
	float Exp = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Progression")
	float Level = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Progression")
	float Gold = 0.f;

	//  전투 스탯
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Combat")
	float Attack = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Combat")
	float Armor = 5.f;

	//  강인도 (Poise)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Combat")
	float Poise = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTag TeamTag;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer AbilityTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer InitialStateTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material")
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> StartupGE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Effect")
	TSubclassOf<UGameplayEffect> GiveTeamtagEffect;
	
#pragma endregion
	
};
