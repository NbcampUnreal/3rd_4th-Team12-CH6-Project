#include "SK_GA_Interaction_Bonfire.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/SKPlayerCharacter.h"
#include "Item/SKInteractableBase.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Interaction_Bonfire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());

	FVector TargetLocation = CachedInteractionData.InteractionLocation;
	TargetLocation.Z =  PlayerCharacter->GetActorLocation().Z;
	const float Distance = GetDistance(PlayerCharacter, TargetLocation);
	const float Duration = Distance/300.0f;

	PlayerCharacter->SetActorRotation(GetTargetRotation(PlayerCharacter, TargetLocation));
	USKActionComponent* ActionComponent = PlayerCharacter->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->SetBonfireWidgetVisibility(CachedTargetActor, false);
	}
	
	MoveToLocation(PlayerCharacter, TargetLocation, Duration);
}

void USK_GA_Interaction_Bonfire::OnMoveCompleted()
{	
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());

	SetForceMove(PlayerCharacter, false);
	
	PlayerCharacter->SetActorRotation(CachedInteractionData.InteractionRotation);
	
	PlayAnimMontage(CachedInteractionData.InteractMontage);
}

void USK_GA_Interaction_Bonfire::OnMontageCompleted()
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	ExecuteTargetInteraction(Cast<UObject>(CachedTargetActor), PlayerCharacter);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}