#include "SK_GA_Step.h"

#include "AbilitySystemComponent.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Step::PreActivateDodge(USKActionComponent* ActionComponent)
{
	Super::PreActivateDodge(ActionComponent);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(TAG_State_Condition_StepBlocked);
	}
	
	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData) return;
	
	UAnimMontage* StepMontage = WeaponAnimData->StepMontage;
	if (!StepMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Step montage is null"));
	}
	DodgeMontage = StepMontage;
}
