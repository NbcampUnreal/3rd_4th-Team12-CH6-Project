// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_StopAttackTrace.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"

void USKAnimNotify_StopAttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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

	PlayerCharacter->StopAttackTrace();

	if (UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent())
	{
		FGameplayEventData EventData;
		EventData.Instigator = PlayerCharacter;
		EventData.Target = PlayerCharacter; 
		EventData.EventTag = FGameplayTag::RequestGameplayTag("Event.LeftATKTraceEnd");
		
		ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
	}
	
}
