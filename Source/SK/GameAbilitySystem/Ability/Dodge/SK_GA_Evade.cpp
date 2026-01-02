#include "SK_GA_Evade.h"

#include "AbilitySystemComponent.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Evade::PreActivateDodge(USKActionComponent* ActionComponent)
{
	Super::PreActivateDodge(ActionComponent);
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(TAG_State_Condition_EvadeBlocked);
	}
	
	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData) return;
	
	UAnimMontage* EvadeMontage = WeaponAnimData->EvadeMontage;
	if (!EvadeMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Evade montage is null"));
	}
	DodgeMontage = EvadeMontage;
}
