// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GCN_Guard_Block.generated.h"

/**
 * 
 */
UCLASS()
class SK_API AGCN_Guard_Block : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
	//AGCN_Guard_Block();

	virtual void HandleGameplayCue(
		AActor* MyTarget,
		EGameplayCueEvent::Type EventType,
		const FGameplayCueParameters& Parameters
	) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	TObjectPtr<UAnimMontage> GuardMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	FName BlockSection = TEXT("Block");
};
