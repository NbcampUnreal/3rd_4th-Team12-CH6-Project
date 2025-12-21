// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SKAnimNotify_CameraShake.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USKAnimNotify_CameraShake : public UAnimNotify
{
	GENERATED_BODY()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;
	
	UPROPERTY(EditAnywhere, Category="SK|CameraShake")
	float VerticalShakeScale = 6.f;

	UPROPERTY(EditAnywhere, Category="SK|CameraShake")
	float HorizonShakeScale = 4.f;

	UPROPERTY(EditAnywhere, Category="SK|CameraShake")
	float Shake_Duration = 12.f;

	UPROPERTY(EditAnywhere, Category="SK|CameraShake")
	float Shake_Frequency = 0.15f;

	
};
