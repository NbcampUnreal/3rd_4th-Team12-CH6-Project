// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "SK_GCN_MeleeAttack.generated.h"


class ASKMeleeHitBox;
/**
 * 
 */
UCLASS()
class SK_API ASK_GCN_MeleeAttack : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	ASK_GCN_MeleeAttack();

	// virtual void OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASKMeleeHitBox> HitboxClass;
};
