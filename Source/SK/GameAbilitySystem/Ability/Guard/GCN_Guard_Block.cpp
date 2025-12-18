// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Guard/GCN_Guard_Block.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"

void AGCN_Guard_Block::HandleGameplayCue(
	AActor* MyTarget,
	EGameplayCueEvent::Type EventType,
	const FGameplayCueParameters& Parameters
)
{
	UE_LOG(LogTemp, Warning, TEXT("Guard block start"));
	
	// 우리는 "한 번 실행"만 필요
	if (EventType != EGameplayCueEvent::Executed) return;

	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(MyTarget);
	if (!Character) return;

	USKActionComponent* ActionComponent = Character->GetActionComponent();
	if (!ActionComponent) return;

	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData) return;
	
	GuardMontage = WeaponAnimData->GuardMontage;
	if (!GuardMontage) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	//여기서 Block 섹션 점프 (모든 클라에서 실행됨)
	AnimInstance->Montage_JumpToSection(BlockSection, GuardMontage);
	UE_LOG(LogTemp, Warning, TEXT("Guard block End"));
}
