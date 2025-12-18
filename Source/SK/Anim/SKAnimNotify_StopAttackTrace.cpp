// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_StopAttackTrace.h"
#include "Component/BattleComponent.h"
#include "Character/SKPlayerCharacter.h"

void USKAnimNotify_StopAttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(MeshComp->GetOwner());
	if (!PC)
		return;

	if (!PC->HasAuthority())
		return;
	
	UBattleComponent* BattleComponent =  PC->GetBattleComponent();
	BattleComponent->Server_StopTrace();
	BattleComponent->Server_ATKTYPE_ApplyDamage(AttackTag);
	
}
