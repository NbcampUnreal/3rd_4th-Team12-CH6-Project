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

	bLeftAttacking = OwnerCharacter->GetIsLeftAttackingByTag();
	LeftComboIndex = OwnerCharacter->GetIsLeftComboIndexByTag();
	
}

void USkAnimInstance_Axe::PlayLeftAttackAnim()
{
	if (LeftAttackMontages.IsValidIndex(LeftComboIndex))
	{
		Montage_Play(LeftAttackMontages[LeftComboIndex]);
		//float Result = Montage_Play(LeftAttackMontages[LeftComboIndex]);
		//UE_LOG(LogTemp, Warning, TEXT("Montage_Play result: %f"), Result);
	}
}

void USkAnimInstance_Axe::EndLeftAttackAnim()
{
	OwnerCharacter->ResetLeftComboState();
}
