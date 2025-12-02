#include "SK_GA_OpenChest.h"
#include "Character/SKPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"

class UAbilityTask_PlayMontageAndWait;

USK_GA_OpenChest::USK_GA_OpenChest()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void USK_GA_OpenChest::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OpenChestAbility!")));
	
	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!SKPlayerCharacter) return;
	SKPlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	FSKInteractionData InteractionData;
	
	USKInteractionComponent* InteractionComponent = SKPlayerCharacter->GetInteractionComponent();
	InteractionComponent->GetInteractionData() = InteractionData;

	if (SKPlayerCharacter->IsLocallyControlled())
	{
		FVector TargetLocation = InteractionData.InteractionLocation;
		TargetLocation.Z = SKPlayerCharacter->GetActorLocation().Z;
	
		SKPlayerCharacter->SetActorLocation(TargetLocation);
		SKPlayerCharacter->SetActorRotation(InteractionData.InteractionRotation);
	}
	
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

	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!SKPlayerCharacter) return;
	SKPlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	
	USKInteractionComponent* InteractionComponent = SKPlayerCharacter->GetInteractionComponent();
	// InteractionComponent->Server_CancelAbility(Handle);
}

void USK_GA_OpenChest::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_OpenChest::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
