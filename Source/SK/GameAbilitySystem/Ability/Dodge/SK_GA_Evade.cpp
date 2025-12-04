#include "SK_GA_Evade.h"
#include "Weapon/ActionData/SKWeaponActionData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Evade::SetAnimMontage(USKActionComponent* ActionComponent)
{
	USKWeaponActionData* WeaponActionData = ActionComponent->GetWeaponActionData();
	if (!WeaponActionData) return;
	
	UAnimMontage* EvadeMontage = WeaponActionData->EvadeMontage;
	if (!EvadeMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Evade montage is null"));
	}
	DodgeMontage = EvadeMontage;
}
