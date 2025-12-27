#include "SK_GA_CombatBase.h"

#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "AbilitySystemComponent.h"

void USK_GA_CombatBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		USKActionComponent* ActionComponent = Character->GetActionComponent();
		if (ActionComponent)
		{
			ActionComponent->OnCombatAction(true);
		}
	}
}
