#include "SK_GA_Step.h"
#include "Weapon/ActionData/SKWeaponActionData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Step::SetAnimMontage(USKActionComponent* ActionComponent)
{
	USKWeaponActionData* WeaponActionData = ActionComponent->GetWeaponActionData();
	if (!WeaponActionData) return;
	
	UAnimMontage* StepMontage = WeaponActionData->StepMontage;
	if (!StepMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Step montage is null"));
	}
	DodgeMontage = StepMontage;
}
