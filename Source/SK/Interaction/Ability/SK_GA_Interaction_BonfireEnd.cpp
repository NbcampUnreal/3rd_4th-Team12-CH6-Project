#include "SK_GA_Interaction_BonfireEnd.h"
#include "Character/SKPlayerCharacter.h"
#include "Item/Openable/Bonfire/SKStool.h"
#include "Item/Openable/Bonfire/SKBonfire.h"
#include "PlayerState/SKPlayerState.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Components/WidgetComponent.h"

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
		ActionComponent->Server_SetIgnoreWorldStatic(false);

		ASKInteractableBase* TargetActor = ActionComponent->InteractedStool;
		if (TargetActor)
		{
			TargetActor->bCanInteract = true;
				
			ASKStool* Stool = Cast<ASKStool>(TargetActor);
			if (Stool)
			{
				ASKBonfire* Bonfire = Stool->OwnerBonfire;
				if (Bonfire)
				{
					TSet<UWidgetComponent*>& DetectWidgets = Bonfire->DetectWidgets;
					for (auto Widget : DetectWidgets)
					{
						Widget->SetVisibility(true);
					}
				}
			}
		}
		ActionComponent->InteractedStool = nullptr;
	}


	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
