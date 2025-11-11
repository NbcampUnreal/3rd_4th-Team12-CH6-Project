#include "SK_GA_Interact.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Interaction/Interface/IInteractable.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetSystemLibrary.h"

USK_GA_Interact::USK_GA_Interact()
	: Distance(200.0f)
{
}

void USK_GA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ActivateAbility()")));

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!SKCharacter) return;
	SKCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Interact"), nullptr);
	PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
	PlayAnimTask->ReadyForActivation();
}

void USK_GA_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("EndAbility()")));

	ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!SKCharacter) return;
	SKCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	UAbilitySystemComponent* ASC = SKCharacter->GetAbilitySystemComponent();
	if (ASC)
	{
		ASC->ClearAbility(Handle);
	}
}

void USK_GA_Interact::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Interact::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
