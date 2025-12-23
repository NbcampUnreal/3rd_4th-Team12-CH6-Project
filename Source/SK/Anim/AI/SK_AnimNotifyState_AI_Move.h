#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SK_AnimNotifyState_AI_Move.generated.h"

UCLASS()
class SK_API USK_AnimNotifyState_AI_Move : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentMontage;

	UPROPERTY()
	TObjectPtr<ACharacter> AI;
	
	float StartNotify = 0.f;
	
	float NotifyDuration = 0.f;
	
	FVector StartLocation = FVector::ZeroVector;
	
	FVector TargetLocation = FVector::ZeroVector;
	
	FHitResult Hit;

	
public:
	USK_AnimNotifyState_AI_Move();
	
protected:
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference
		) override;

	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float FrameDeltaTime,
		const FAnimNotifyEventReference& EventReference
		) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
		) override;
};
