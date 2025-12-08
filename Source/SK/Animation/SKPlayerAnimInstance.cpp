#include "SKPlayerAnimInstance.h"

#include "Character/SKPlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void USKPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ASKPlayerCharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void USKPlayerAnimInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USKPlayerAnimInstance, CurrentMoveDirection);
	DOREPLIFETIME(USKPlayerAnimInstance, bIsLockOn);
}
