// Fill out your copyright notice in the Description page of Project Settings.


#include "SK_GA_SimpleInteract.h"

#include "Abilities/tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/SKPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"

USK_GA_SimpleInteract::USK_GA_SimpleInteract()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void USK_GA_SimpleInteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char && Char->HasAuthority()) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); return; }
	
	UE_LOG(LogTemp, Warning, TEXT("Active Simple Interact Ability, %s"), Char->HasAuthority() ? TEXT("Server") : TEXT("Client"));

	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); return; }

	FSKInteractionData& Data = InteractionComponent->GetInteractionData();
	
	if (!Data.InteractMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Have Montage, %s"), Char->HasAuthority() ? TEXT("Server") : TEXT("Client"));
	}
	if (Data.InteractMontage)
	{
		
		Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Interact"), Data.InteractMontage);
		PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
		PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAnimTask->ReadyForActivation();
	}
}

void USK_GA_SimpleInteract::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("End Simple Interact Ability"));

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char) return;
	Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	
	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent) return;

	InteractionComponent->SetInteractionData(FSKInteractionData());
}

void USK_GA_SimpleInteract::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_SimpleInteract::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
