#include "SKPlayerAnimInstance.h"

#include "Character/SKPlayerCharacter.h"

void USKPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ASKPlayerCharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void USKPlayerAnimInstance::PlayLeftAttackAnim()
{
	if (LeftAttackMontages.IsValidIndex(LeftComboIndex))
	{
		//Montage_Play(LeftAttackMontages[LeftComboIndex]);
	}
}



void USKPlayerAnimInstance::SetComboIndex(int32 ComboIndex)
{
	LeftComboIndex = ComboIndex;
}

void USKPlayerAnimInstance::SetIsAttacking(bool isAttacking)
{
	bLeftAttacking = isAttacking;
}

TArray<UAnimMontage*> USKPlayerAnimInstance::GetLeftATKMontage()
{
	return LeftAttackMontages;
}

UAnimMontage* USKPlayerAnimInstance::GetLeftATKMontageIndex()
{
	if (LeftAttackMontages.IsValidIndex(LeftComboIndex))
	{
		return LeftAttackMontages[LeftComboIndex];
	}
	return nullptr;
}
