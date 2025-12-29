#include "SKAN_SwitchTransform.h"

#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

void USKAN_SwitchTransform::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(MeshComp->GetOwner());
	if (!Char)
	{
		return;
	}

	USKActionComponent* ActionComponent = Char->GetActionComponent();
	if (!ActionComponent)
	{
		return;
	}
	AActor* Weapon = ActionComponent->WeaponActors[0];
	Weapon->SetActorRelativeRotation(FRotator(-30.f, 0, 180.f));
}
