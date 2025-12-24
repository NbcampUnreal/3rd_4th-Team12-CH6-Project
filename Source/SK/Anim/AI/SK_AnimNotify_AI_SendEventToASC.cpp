#include "Anim/AI/SK_AnimNotify_AI_SendEventToASC.h"
#include "Controller/AI/SKAIController.h"
#include "GameFramework/Character.h"

USK_AnimNotify_AI_SendEventToASC::USK_AnimNotify_AI_SendEventToASC()
{
	
}

void USK_AnimNotify_AI_SendEventToASC::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	ACharacter* Owner = Cast<ACharacter>(MeshComp->GetOwner());
	if (!IsValid(Owner))
	{
		return;
	}

	ASKAIController* AIController = Cast<ASKAIController>(Owner->GetController());
	if (!IsValid(AIController))
	{
		return;
	}

	AIController->SendEventToASC(nullptr, nullptr, EventTag);
}