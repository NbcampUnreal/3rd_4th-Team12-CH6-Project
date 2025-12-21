#include "SKAN_UnequipWeapon.h"

#include "Character/SKPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USKAN_UnequipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(MeshComp->GetOwner());
	if (!Char)
	{
		return;
	}
		
	USKActionComponent* ActionComponent = Char->GetActionComponent();
	if (!ActionComponent)
	{
		return;
	}

	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (ASC)
	{
		const FGameplayTag EquipTag = FGameplayTag::RequestGameplayTag(TEXT("State.Condition.Equip"));
		// ASC->RemoveLooseGameplayTag(EquipTag);
		
		USKWeaponAnimData* AnimData = ActionComponent->GetWeaponAnimData();
		if (!AnimData) return;
		
		if (AnimData->UnequipGE && AnimData->EquipGE)
		{
			ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->UnequipGE, ASC, 1);
			ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->EquipGE, ASC, 1);
			
			FGameplayEffectSpecHandle UnequipGESpecHandle = ASC->MakeOutgoingSpec(AnimData->UnequipGE, 1.f, ASC->MakeEffectContext());
			if (UnequipGESpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*UnequipGESpecHandle.Data.Get());
			}
		}
	}

	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData) return;

	TArray<FName> UnequipSocketName = WeaponAnimData->UnequipSocketName;

	ActionComponent->AttachWeapon(UnequipSocketName);
}
