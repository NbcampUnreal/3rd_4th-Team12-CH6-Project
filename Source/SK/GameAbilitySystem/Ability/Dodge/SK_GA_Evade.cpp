#include "SK_GA_Evade.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Evade::PreActivateDodge(USKActionComponent* ActionComponent)
{
	USKWeaponAnimData* WeaponActionData = ActionComponent->GetWeaponActionData();
	if (!WeaponActionData) return;
	
	UAnimMontage* EvadeMontage = WeaponActionData->EvadeMontage;
	if (!EvadeMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Evade montage is null"));
	}
	DodgeMontage = EvadeMontage;
}
