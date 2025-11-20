// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "EquipmentInstance.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FGameplayAbilitySpecHandle GrantedAbilityHandle;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FActiveGameplayEffectHandle GrantedEffectHandle;
};
