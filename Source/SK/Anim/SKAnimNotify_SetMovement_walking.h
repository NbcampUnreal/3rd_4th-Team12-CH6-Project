// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Anim/SKAnimNotifyBase.h"
#include "SKAnimNotify_SetMovement_walking.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USKAnimNotify_SetMovement_walking : public USKAnimNotifyBase
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
