// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_PlaySound.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void USKAnimNotify_PlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UAbilitySystemComponent* ASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

	if (!ASC) return;

	FGameplayEventData EventData;
	EventData.EventTag = GameplayCueTag;
	EventData.Instigator = Owner;
	EventData.Target = Owner;


	ASC->ExecuteGameplayCue(GameplayCueTag, FGameplayCueParameters());
}
