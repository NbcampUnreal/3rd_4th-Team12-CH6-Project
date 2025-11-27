#include "Anim/AI/SK_AnimNotify_AI_Snapshot.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "GameAbilitySystem/GameplayEffect/AI/SK_GE_AddTagDuration.h"

USK_AnimNotify_AI_Snapshot::USK_AnimNotify_AI_Snapshot()
{
	
}

void USK_AnimNotify_AI_Snapshot::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	MeshComp->GetAnimInstance()->SavePoseSnapshot(TEXT("PoseSnapshot"));
	
	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}
	
	UAbilitySystemComponent* OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!OwnerASC)
	{
		return;
	}

	OwnerASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Condition.Pause")));
	/*
	FGameplayEffectContextHandle Context = OwnerASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(USK_GE_AddTagDuration::StaticClass(), 1.0f, Context);
	if (!SpecHandle.IsValid())
	{
		return;
	}

	OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	*/
}
