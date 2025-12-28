// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "Utility/HitReactUtils.h"
#include "GC_HitReact.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UGC_HitReact : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	virtual bool OnExecute_Implementation(
		AActor* Target,
		const FGameplayCueParameters& Parameters
	) const override;

private:
	EHitReactAnim GetHitReactDirectionFromTag(
		const FGameplayTag& CueTag
	) const;
};
