#include "AN_RemoveRollTag.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Utility/SKNativeGameplayTags.h"


void UAN_RemoveRollTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(MeshComp->GetOwner());
	if (!Character) return;
	
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!ASC) return;

	ASC->RemoveLooseGameplayTag(TAG_State_Condition_StepBlocked);
	ASC->RemoveLooseGameplayTag(TAG_State_Condition_EvadeBlocked);
}
