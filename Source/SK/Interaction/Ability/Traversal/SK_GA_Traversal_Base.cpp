#include "SK_GA_Traversal_Base.h"

#include "Animation/SKBaseAnimInstance.h"
#include "Character/SKPlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Item/SKInteractableBase.h"
#include "Item/Traversal/SKTraversalBase.h"

void USK_GA_Traversal_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	FVector TargetLocation = CachedInteractionData.InteractionLocation;
	TargetLocation.Z =  PlayerCharacter->GetActorLocation().Z;
	const float Distance = GetDistance(PlayerCharacter, TargetLocation);
	const float Duration = Distance/300.0f;

	if (Distance > 30.0f)
	{
		PlayerCharacter->SetActorRotation(GetTargetRotation(PlayerCharacter, TargetLocation));
	}
	
	USKActionComponent* ActionComponent = PlayerCharacter->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->Server_SetIgnoreCollision(true);
	}
	
	MoveToLocation(PlayerCharacter, TargetLocation, Duration, EForceMoveMode::Normal);
	
}

void USK_GA_Traversal_Base::SetMovementMode(ACharacter* Character, bool bIsFlying)
{
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
	if (MovementComponent)
	{
		MovementComponent->SetMovementMode(bIsFlying? MOVE_Flying : MOVE_Walking);
	}
}

void USK_GA_Traversal_Base::SetEndAbility()
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	USKActionComponent* ActionComponent = PlayerCharacter->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->Server_SetIgnoreCollision(false);
	}
	
	SetMovementMode(PlayerCharacter, false);
	CachedTargetActor->bCanInteract = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Traversal_Base::OnMoveCompleted()
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());

	SetForceMove(PlayerCharacter, false, EForceMoveMode::Normal);

	SetMovementMode(PlayerCharacter, true);
	
	PlayerCharacter->SetActorRotation(CachedInteractionData.InteractionRotation);
	
	PlayAnimMontage(CachedInteractionData.InteractMontage);
}

void USK_GA_Traversal_Base::OnMontageCompleted()
{
	SetEndAbility();
}

void USK_GA_Traversal_Base::OnMontageCanceled()
{
	SetEndAbility();
}
