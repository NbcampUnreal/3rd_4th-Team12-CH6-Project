#include "Animation/AI/SKAIBaseAnimInstance.h"
#include "Character/AI/SKAICharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

USKAIBaseAnimInstance::USKAIBaseAnimInstance()
{
	
}

void USKAIBaseAnimInstance::SetbIsFlying(bool NewValue)
{
	bIsFlying = NewValue;
}

void USKAIBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ASKAICharacterBase>(GetOwningActor()); 

	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	MovementComponent = OwnerCharacter->GetCharacterMovement();
}

void USKAIBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MovementComponent == nullptr)
	{
		if (!IsValid(OwnerCharacter))
		{
			return;
		}
		
		MovementComponent = OwnerCharacter->GetCharacterMovement();
		if (MovementComponent == nullptr)
		{
			return;
		}
	}
	
	FVector Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D(); 

	bIsMoving = GroundSpeed > 3.0f; 
    
	bIsFalling = MovementComponent->IsFalling();
}
