#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SKAIBaseAnimInstance.generated.h"

class UCharacterMovementComponent;

UCLASS()
class SK_API USKAIBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reference")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reference")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed; 

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsMoving;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFlying;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "PoseSnapshot")
	FPoseSnapshot PoseSnapshot;

public:
	USKAIBaseAnimInstance();

	void SetbIsFlying(bool NewValue);
	
protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
