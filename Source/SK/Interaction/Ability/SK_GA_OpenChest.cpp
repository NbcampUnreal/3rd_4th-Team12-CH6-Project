#include "SK_GA_OpenChest.h"
#include "Character/SKPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"

class UAbilityTask_PlayMontageAndWait;

USK_GA_OpenChest::USK_GA_OpenChest()
{
}

void USK_GA_OpenChest::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ActivateAbility()")));

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!SKCharacter) return;
	SKCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	if (!OpenAnimMontage) return;
	UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Interact"), OpenAnimMontage);
	PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
	PlayAnimTask->ReadyForActivation();
}

void USK_GA_OpenChest::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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

void USK_GA_OpenChest::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_OpenChest::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
