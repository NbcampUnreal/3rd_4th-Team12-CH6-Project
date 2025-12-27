#include "SK_GA_Traversal_AvoidObstacle.h"
#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Item/SKInteractableBase.h"
#include "Item/Traversal/SKTraversalBase.h"

void USK_GA_Traversal_AvoidObstacle::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	FVector TargetLocation = CachedInteractionData.InteractionLocation;
	TargetLocation.Z =  PlayerCharacter->GetActorLocation().Z;
	const float Distance = GetDistance(PlayerCharacter, TargetLocation);
	const float Duration = Distance/150.0f;

	PlayerCharacter->SetActorRotation(GetTargetRotation(PlayerCharacter, TargetLocation));
	
	USKActionComponent* ActionComponent = PlayerCharacter->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->Server_SetIgnoreCollision(true);
	}

	ASKTraversalBase* TraversalObject = Cast<ASKTraversalBase>(CachedTargetActor);
	if (TraversalObject)
	{
		MoveToLocation(PlayerCharacter, TargetLocation, Duration, TraversalObject->ForceMoveMode);
	}
}

void USK_GA_Traversal_AvoidObstacle::SetEndAbility()
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	USKActionComponent* ActionComponent = PlayerCharacter->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->Server_SetIgnoreCollision(false);
	}

	ASKTraversalBase* TraversalObject = Cast<ASKTraversalBase>(CachedTargetActor);
	if (TraversalObject)
	{
		SetForceMove(PlayerCharacter, false, TraversalObject->ForceMoveMode);
	}
	
	CachedTargetActor->bCanInteract = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Traversal_AvoidObstacle::OnMoveCompleted()
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());

	SetEndAbility();
}