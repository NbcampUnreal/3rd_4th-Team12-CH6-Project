#include "SKAN_SwitchTransform.h"

#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USKAN_SwitchTransform::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	USKActionComponent* ActionComponent = Character->GetActionComponent();
	if (!ActionComponent)
	{
		return;
	}
	AActor* Weapon = ActionComponent->WeaponActors[0];
	Weapon->SetActorRelativeRotation(FRotator(-30.f, 0, 180.f));
}
