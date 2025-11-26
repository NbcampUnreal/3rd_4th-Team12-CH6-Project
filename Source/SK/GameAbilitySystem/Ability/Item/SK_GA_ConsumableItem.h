// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_ConsumableItem.generated.h"

class USKInventoryItemData;
/**
 * 
 */
UCLASS()
class SK_API USK_GA_ConsumableItem : public USKGameplayAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	USKInventoryItemData* GetItemDataFromItemID() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ConsumableItem")
	int32 ItemID;	
};
