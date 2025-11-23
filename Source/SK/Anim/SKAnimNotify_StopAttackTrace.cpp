// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_StopAttackTrace.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "GameplayAbilitySpec.h"
#include "GameAbilitySystem/Ability/SK_GA_LeftAttack_Axe.h"

void USKAnimNotify_StopAttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(MeshComp->GetOwner());
	if (!PC)
		return;

	if (!PC->HasAuthority())
		return;

	PC->Server_Notify_StopAttackTrace();
	// PlayerCharacter->StopAttackTrace();
	//
	// if (UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent())
	// {
	// 	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	// 	{
	// 		if (Spec.Ability && Spec.Ability->GetClass()->IsChildOf(USK_GA_LeftAttack_Axe::StaticClass()))
	// 		{
	// 			USK_GA_LeftAttack_Axe* GA = Cast<USK_GA_LeftAttack_Axe>(Spec.GetPrimaryInstance());
	// 			if (GA)
	// 			{
	// 				GA->ApplyDamageFromTrace();
	// 				GA->EndAbility(Spec.Handle, ASC->AbilityActorInfo.Get(), Spec.ActivationInfo, true, false);
	// 			}
	// 			break;
	// 		}
	// 	}
	// }
}
