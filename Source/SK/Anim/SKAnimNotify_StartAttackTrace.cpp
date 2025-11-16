// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_StartAttackTrace.h"

#include "GameplayTagContainer.h"
#include "Character/SKPlayerCharacter.h"

class ASKPlayerCharacter;

void USKAnimNotify_StartAttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimNotify failed to get Owner Actor."));
		return; 
	}

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(OwnerActor);
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimNotify failed to cast Owner Actor to ASKPlayerCharacter."));
		return; 
	}

	PlayerCharacter->StartAttackTrace();
	
}
