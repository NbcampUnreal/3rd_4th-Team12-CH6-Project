#include "SK_GA_Interaction_Openable.h"
#include "Character/SKPlayerCharacter.h"
#include "Item/SKInteractableBase.h"

void USK_GA_Interaction_Openable::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	
	MoveToLocation(PlayerCharacter, TargetLocation, Duration);
	
}

void USK_GA_Interaction_Openable::OnMoveCompleted()
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());

	SetForceMove(PlayerCharacter, false);
	
	ExecuteTargetInteraction(Cast<UObject>(CachedTargetActor), PlayerCharacter);

	PlayerCharacter->SetActorRotation(CachedInteractionData.InteractionRotation);
	
	PlayAnimMontage(CachedInteractionData.InteractMontage);
}
