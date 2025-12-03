#include "SK_GA_Evade.h"
#include "Component/SKCombatComponent.h"
#include "Weapon/StateData/SKWeaponStateData.h"

void USK_GA_Evade::SetAnimMontage(USKCombatComponent* CombatComponent)
{
	UAnimMontage* EvadeMontage = CombatComponent->CurrentWeaponStateData->EvadeMontage;
	if (!EvadeMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Evade montage is null"));
	}
	DodgeMontage = EvadeMontage;
}
