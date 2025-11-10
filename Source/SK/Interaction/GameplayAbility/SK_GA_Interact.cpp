#include "SK_GA_Interact.h"

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

	UCameraComponent* CameraComponent = SKCharacter->GetFollowCamera();

	FVector Start = SKCharacter->GetActorLocation();
	
	FVector Direction = CameraComponent->GetForwardVector();
	Direction.Z = 0.f;
	Direction.Normalize();

	FVector End = Start + Direction * Distance;

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(SKCharacter);

	FInteractionData InteractionData;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			IInteractable::Execute_GetInteractionData(HitActor, InteractionData);

			IInteractable::Execute_Interact(HitActor, SKCharacter);
		}
	}

	UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Interact"), InteractionData.AnimationMontage);
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
}

void USK_GA_Interact::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Interact::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
