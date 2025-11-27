// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SkAnimInstance_Axe.h"
#include "Character/SKPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void USkAnimInstance_Axe::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ASKPlayerCharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void USkAnimInstance_Axe::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(OwnerCharacter) == false || IsValid(OwnerCharacterMovementComponent) == false)
	{
		return;
	}

	Velocity = OwnerCharacterMovementComponent->GetLastUpdateVelocity();   
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();

	bShouldMove = ((OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) == false)
	&& (3.f < GroundSpeed);
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();

	OwnerCharacter = Cast<ASKPlayerCharacter>(GetOwningActor());
	

	
}

void USkAnimInstance_Axe::PlayLeftAttackAnim()
{
	if (LeftAttackMontages.IsValidIndex(LeftComboIndex))
	{
		//Montage_Play(LeftAttackMontages[LeftComboIndex]);
	}
}



void USkAnimInstance_Axe::SetComboIndex(int32 ComboIndex)
{
	LeftComboIndex = ComboIndex;
}

void USkAnimInstance_Axe::SetIsAttacking(bool isAttacking)
{
	bLeftAttacking = isAttacking;
}

TArray<UAnimMontage*> USkAnimInstance_Axe::GetLeftATKMontage()
{
	return LeftAttackMontages;
}

UAnimMontage* USkAnimInstance_Axe::GetLeftATKMontageIndex()
{
	if (LeftAttackMontages.IsValidIndex(LeftComboIndex))
	{
		return LeftAttackMontages[LeftComboIndex];
	}
	return nullptr;
}




