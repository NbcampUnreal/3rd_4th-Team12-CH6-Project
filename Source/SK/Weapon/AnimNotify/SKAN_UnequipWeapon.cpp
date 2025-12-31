#include "SKAN_UnequipWeapon.h"

#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USKAN_UnequipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}
		
	USKActionComponent* ActionComponent = Character->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->OnCombatAction(false);
	}
}
