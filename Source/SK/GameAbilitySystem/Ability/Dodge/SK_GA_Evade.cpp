#include "SK_GA_Evade.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Evade::PreActivateDodge(USKActionComponent* ActionComponent)
{
	Super::PreActivateDodge(ActionComponent);
	
	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData) return;
	
	UAnimMontage* EvadeMontage = WeaponAnimData->EvadeMontage;
	if (!EvadeMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Evade montage is null"));
	}
	DodgeMontage = EvadeMontage;
}
