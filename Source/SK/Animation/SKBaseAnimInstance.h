#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SKBaseAnimInstance.generated.h"

class ASKCharacterBase;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class EForceMoveMode : uint8
{
	Normal,
	Crouch,
	GoUnder
};

UCLASS()
class SK_API USKBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	USKBaseAnimInstance();
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ASKCharacterBase> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsFalling : 1;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 bIsSit : 1;
};
