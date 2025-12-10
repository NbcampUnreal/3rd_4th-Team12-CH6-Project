#include "SK_GA_Step.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Step::PreActivateDodge(USKActionComponent* ActionComponent)
{
	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData) return;
	
	UAnimMontage* StepMontage = WeaponAnimData->StepMontage;
	if (!StepMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Step montage is null"));
	}
	DodgeMontage = StepMontage;
}
