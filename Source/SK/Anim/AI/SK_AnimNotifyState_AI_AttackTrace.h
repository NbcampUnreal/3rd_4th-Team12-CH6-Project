#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SK_AnimNotifyState_AI_AttackTrace.generated.h"

UCLASS()
class SK_API USK_AnimNotifyState_AI_AttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	// 이걸 몽타주 안 노티파이스테이트 디테일에서 지정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace")
	TArray<FName> SocketNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace")
	float CapsuleRadius = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace")
	float CapsuleHalfHeight = 20.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trace")
	TArray<AActor*> IgnoreActors;
	
	TMap<FName, FVector> PrevSocketLocations;

public:
	USK_AnimNotifyState_AI_AttackTrace();
	
protected:
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference
		) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
		) override;

	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float FrameDeltaTime,
		const FAnimNotifyEventReference& EventReference
		) override;
};
