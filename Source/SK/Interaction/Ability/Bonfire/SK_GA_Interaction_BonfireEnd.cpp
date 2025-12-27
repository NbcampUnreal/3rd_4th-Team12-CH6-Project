#include "SK_GA_Interaction_BonfireEnd.h"
#include "Character/SKPlayerCharacter.h"
#include "PlayerState/SKPlayerState.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Interaction_BonfireEnd::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	PlayAnimMontage(BonfireEndMontage);
}

void USK_GA_Interaction_BonfireEnd::OnMoveCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Interaction_BonfireEnd::OnMontageCompleted()
{	
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	USKActionComponent* ActionComponent = PlayerCharacter->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->SetBonfireWidgetVisibility(CachedTargetActor, true);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
