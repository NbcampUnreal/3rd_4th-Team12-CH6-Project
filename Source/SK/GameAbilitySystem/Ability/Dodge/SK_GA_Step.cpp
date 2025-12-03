#include "SK_GA_Step.h"
#include "Component/SKCombatComponent.h"
#include "Weapon/ActionData/SKWeaponActionData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Step::SetAnimMontage(USKActionComponent* ActionComponent)
{
	UAnimMontage* StepMontage = ActionComponent->GetWeaponActionData()->StepMontage;
	if (!StepMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Step montage is null"));
	}
	DodgeMontage = StepMontage;
}
