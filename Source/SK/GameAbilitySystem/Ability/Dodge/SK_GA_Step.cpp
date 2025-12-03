#include "SK_GA_Step.h"
#include "Component/SKCombatComponent.h"
#include "Weapon/ActionData/SKWeaponActionData.h"

void USK_GA_Step::SetAnimMontage(USKCombatComponent* CombatComponent)
{
	UAnimMontage* StepMontage = CombatComponent->CurrentWeaponStateData->StepMontage;
	if (!StepMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Step montage is null"));
	}
	DodgeMontage = StepMontage;
}
