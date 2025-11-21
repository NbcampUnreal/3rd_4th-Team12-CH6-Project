#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SKAnimNotifyState.generated.h"

UCLASS()
class SK_API USKAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	TArray<FName> SocketNames { TEXT("left_hand_socket"), TEXT("right_hand_socket") };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	float CapsuleRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	float CapsuleHalfHeight = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	TArray<AActor*> IgnoreActors;
	
	TMap<FName, FVector> PrevSocketLocations;

public:
	USKAnimNotifyState();
	
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
