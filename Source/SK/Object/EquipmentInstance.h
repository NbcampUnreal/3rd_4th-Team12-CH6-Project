// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "EquipmentInstance.generated.h"

struct FSKEquipmentActorToSpawn;
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

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FGameplayTag EquipTag;
	
	// 캐릭터에 장비 액터 스폰
	UFUNCTION(BlueprintCallable)
	void SpawnEquipmentActors(APawn* OwningPawn, const TArray<FSKEquipmentActorToSpawn>& ActorsToSpawn);

	// 장비 액터 제거
	UFUNCTION(BlueprintCallable)
	void DestroyEquipmentActors();
	
protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<AActor*> SpawnedActors;
};
