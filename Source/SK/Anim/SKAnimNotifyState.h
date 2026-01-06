// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SKAnimNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USKAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootStep")
	FGameplayTag FootStepSoundTag;

	/** 최소 속도 (이하일 경우 소리 안 남) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootStep")
	float MinMoveSpeed = 10.f;

	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration) override;

	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float FrameDeltaTime) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation) override;

private:
	/** 내부 쿨타임 */
	float ElapsedTime = 0.f;

	/** 발소리 간격 */
	float StepInterval = 0.35f;
	
};
