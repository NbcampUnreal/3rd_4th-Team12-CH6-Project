// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_ResetLeftAttack.h"
#include "Component/BattleComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Controller/SKPlayerController.h"

void USKAnimNotify_ResetLeftAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!IsValid(OwnerActor))
	{
		//UE_LOG(LogTemp, Warning, TEXT("AnimNotify failed to get Owner Actor."));
		return; 
	}

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(OwnerActor);
	if (!IsValid(PlayerCharacter))
	{
		//UE_LOG(LogTemp, Warning, TEXT("AnimNotify failed to cast Owner Actor to ASKPlayerCharacter."));
		return; 
	}

	ASKPlayerController* Controller = Cast<ASKPlayerController>(PlayerCharacter->GetController());
	if (Controller)
	{
		Controller->SetCanMaintainCombo(true);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player Controller Not Found"));
	}
	//
	// USKCombatComponent* CombatComponent =  PlayerCharacter->GetCombatComponent();
	// CombatComponent->Server_OnATKEndNotify(true);
	UBattleComponent* BattleComponent =  PlayerCharacter->GetBattleComponent();
	BattleComponent->Server_OnATKEndNotify(true);
}
