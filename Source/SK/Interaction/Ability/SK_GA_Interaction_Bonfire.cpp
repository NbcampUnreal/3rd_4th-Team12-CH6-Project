#include "SK_GA_Interaction_Bonfire.h"
#include "Character/SKPlayerCharacter.h"
#include "Item/SKInteractableBase.h"

void USK_GA_Interaction_Bonfire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());

	FVector TargetLocation = GetTargetLocation(PlayerCharacter);
	TargetLocation.Z = PlayerCharacter->GetActorLocation().Z; 
	
	const float Distance = GetDistance(PlayerCharacter, TargetLocation);
	const float Duration = Distance/300.0f;

	if (Distance < DistanceThreshold)
	{
		FVector NewTargetLocation = CachedTargetActor->GetActorLocation();
		NewTargetLocation.Z = PlayerCharacter->GetActorLocation().Z;
		PlayerCharacter->SetActorRotation(GetTargetRotation(PlayerCharacter, NewTargetLocation));
		OnMoveCompleted();
	}
	else
	{
		PlayerCharacter->SetActorRotation(GetTargetRotation(PlayerCharacter, TargetLocation));
		MoveToLocation(PlayerCharacter, TargetLocation, Duration);
	}
	
}

void USK_GA_Interaction_Bonfire::OnMoveCompleted()
{	
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());

	SetForceMove(PlayerCharacter, false);
	
	PlayAnimMontage(CachedInteractionData.InteractMontage);
}

void USK_GA_Interaction_Bonfire::OnMontageCompleted()
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	ExecuteTargetInteraction(Cast<UObject>(CachedTargetActor), PlayerCharacter);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

FVector USK_GA_Interaction_Bonfire::GetTargetLocation(const ASKPlayerCharacter* PlayerCharacter) const
{
	const FVector TargetLocation = CachedTargetActor->GetActorLocation();
	const FVector CharacterLocation = PlayerCharacter->GetActorLocation();
	
	FVector FlatTarget = TargetLocation;
	FVector FlatCharacter = CharacterLocation;
	FlatTarget.Z = 0.f;
	FlatCharacter.Z = 0.f;
	
	FVector Direction = (FlatCharacter - FlatTarget).GetSafeNormal();
	
	return TargetLocation + Direction * TargetDistance;
}
