#include "SK_GA_Interact.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Interaction/Interface/SKInteractable.h"
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

	FVector End = Start + Direction * 200.0f;

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(SKCharacter);

	FSKInteractionData InteractionData;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(USKInteractable::StaticClass()))
		{
			ISKInteractable::Execute_GetInteractionData(HitActor, InteractionData);
			ISKInteractable::Execute_Interact(HitActor, SKCharacter);
		}
	}

	else
	{
		UAbilitySystemComponent* ASC = SKCharacter->GetAbilitySystemComponent();
		if (!ASC) return;

		if (InteractionData.GrantedAbility)
		{
			FGameplayAbilitySpecHandle NewHandle = ASC->GiveAbility(
				FGameplayAbilitySpec(InteractionData.GrantedAbility, 1, INDEX_NONE, this)
				);
			ASC->TryActivateAbility(NewHandle);
		}
	}
}

void USK_GA_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
