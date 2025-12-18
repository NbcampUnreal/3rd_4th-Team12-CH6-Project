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
	
	// USKCombatComponent* CombatComponent =  PC->GetCombatComponent();
	// CombatComponent->Server_StopTrace();
	// CombatComponent->Server_Notify_StopAttackTrace();
	UBattleComponent* BattleComponent =  PC->GetBattleComponent();
	BattleComponent->Server_StopTrace();
	BattleComponent->Server_LeftATK_ApplyDamage();
}
