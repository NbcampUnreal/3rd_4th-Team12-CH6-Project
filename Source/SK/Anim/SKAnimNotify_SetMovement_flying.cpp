// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_SetMovement_flying.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void USKAnimNotify_SetMovement_flying::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character)
		return;

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp)
		return;

	// ★ MovementMode를 Walking으로 변경
	MoveComp->SetMovementMode(MOVE_Flying);
	
}
