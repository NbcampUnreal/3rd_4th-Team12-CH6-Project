#include "SK_GA_CombatBase.h"

#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
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
			UE_LOG(LogTemp, Display, TEXT("OnCombatAction"));

			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			if (!ASC) return;

			const FGameplayTag EquipTag = FGameplayTag::RequestGameplayTag(TEXT("State.Condition.Equip"));

			// ASC->RemoveLooseGameplayTag(EquipTag);
			// ASC->AddLooseGameplayTag(EquipTag);

			USKWeaponAnimData* AnimData = ActionComponent->GetWeaponAnimData();
			if (!AnimData) return;
		
			if (AnimData->UnequipGE && AnimData->EquipGE)
			{
				ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->UnequipGE, ASC, 1);
				ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->EquipGE, ASC, 1);
				
				FGameplayEffectSpecHandle UnequipGESpecHandle = ASC->MakeOutgoingSpec(AnimData->EquipGE, 1.f, ASC->MakeEffectContext());
				if (UnequipGESpecHandle.IsValid())
				{
					ASC->ApplyGameplayEffectSpecToSelf(*UnequipGESpecHandle.Data.Get());
				}
			}
			
			ActionComponent->AttachWeapon(ActionComponent->GetWeaponAnimData()->EquipSocketName);

			ActionComponent->LastCombatTime = GetWorld()->GetTimeSeconds();
		}
	}
}
