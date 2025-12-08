#include "SKPlayerAnimInstance.h"

#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USKPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerPlayer = Cast<ASKPlayerCharacter>(GetOwningActor());
	ActionComponent = OwnerPlayer->GetActionComponent();
}

void USKPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(OwnerPlayer)) return;
	// bIsLockedOn = OwnerPlayer->bIsLockedOn;
	
	if (!IsValid(ActionComponent)) return;
	// CurrentMoveDirection = ActionComponent->CurrentMoveDirection;
}
