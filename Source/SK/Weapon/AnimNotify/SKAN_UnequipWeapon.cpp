#include "SKAN_UnequipWeapon.h"

#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USKAN_UnequipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(MeshComp->GetOwner());
	if (!Char)
	{
		return;
	}
		
	USKActionComponent* ActionComponent = Char->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->OnCombatAction(false);
	}
}
