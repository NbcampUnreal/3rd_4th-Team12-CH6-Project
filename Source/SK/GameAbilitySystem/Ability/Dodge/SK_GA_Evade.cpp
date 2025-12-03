#include "SK_GA_Evade.h"
#include "Weapon/ActionData/SKWeaponActionData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USK_GA_Evade::SetAnimMontage(USKActionComponent* ActionComponent)
{
	UAnimMontage* EvadeMontage = ActionComponent->GetWeaponActionData()->EvadeMontage;
	if (!EvadeMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Evade montage is null"));
	}
	DodgeMontage = EvadeMontage;
}
