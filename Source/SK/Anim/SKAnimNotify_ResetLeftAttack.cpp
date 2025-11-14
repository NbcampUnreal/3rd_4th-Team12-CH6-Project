// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_ResetLeftAttack.h"

#include "Character/SKPlayerCharacter.h"

void USKAnimNotify_ResetLeftAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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

	FGameplayTag CurrentTag = PlayerCharacter->CurrentWeaponTag;

	PlayerCharacter->ResetLeftComboState(CurrentTag);
}
