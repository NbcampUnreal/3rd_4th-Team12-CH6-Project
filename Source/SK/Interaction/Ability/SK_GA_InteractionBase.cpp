// Fill out your copyright notice in the Description page of Project Settings.


#include "SK_GA_InteractionBase.h"

#include "Character/SKPlayerCharacter.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"
#include "Interaction/Interface/SKInteractable.h"

USK_GA_InteractionBase::USK_GA_InteractionBase()
	: CachedTargetActor(nullptr)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void USK_GA_InteractionBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
			
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Active Interaction Ability, %s"), Char->HasAuthority() ? TEXT("Server") : TEXT("Client"));

	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	ASKInteractableBase* TargetActor = InteractionComponent->GetCurrentTargetActor();
	CachedTargetActor = TargetActor;
	CachedInteractionData = InteractionComponent->GetInteractionData();
}

void USK_GA_InteractionBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("End Interaction Ability"));

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char) return;
	
	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent) return;

	InteractionComponent->SetInteractionData(FSKInteractionData());
	CachedInteractionData = FSKInteractionData();
}

void USK_GA_InteractionBase::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_InteractionBase::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
