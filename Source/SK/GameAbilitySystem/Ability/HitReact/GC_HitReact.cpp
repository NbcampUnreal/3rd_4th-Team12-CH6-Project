// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/HitReact/GC_HitReact.h"
#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"

bool UGC_HitReact::OnExecute_Implementation(
	AActor* Target,
	const FGameplayCueParameters& Parameters
) const
{
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(Target);
	if (!Character) return false;
	
	USKActionComponent* ActionComponent = Character->GetActionComponent();
	if (!ActionComponent) return false;

	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData) return false;

	
	// Cue Tag → HitReact 방향 변환
	const EHitReactAnim Direction = GetHitReactDirectionFromTag(Parameters.OriginalTag);

	// 방향에 맞는 몽타주 선택
	UAnimMontage* MontageToPlay = nullptr;

	switch (Direction)
	{
	case EHitReactAnim::FrontLeft:
		MontageToPlay = WeaponAnimData->HitReactMontages.FrontLeft;
		break;

	case EHitReactAnim::FrontRight:
		MontageToPlay = WeaponAnimData->HitReactMontages.FrontRight;
		break;

	case EHitReactAnim::Back:
		MontageToPlay = WeaponAnimData->HitReactMontages.Back;
		break;
	}

	if (!MontageToPlay)
		return false;

	// 몽타주 재생
	UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance();
	if (!Anim || Anim->IsAnyMontagePlaying()) return false;

	Anim->Montage_Play(MontageToPlay);
	return true;
}

EHitReactAnim UGC_HitReact::GetHitReactDirectionFromTag(
	const FGameplayTag& CueTag
) const
{
	UE_LOG(LogTemp, Warning, TEXT("[GC_HitReact] CueTag : %s"), *CueTag.GetTagName().ToString());
	
	if (CueTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.HitReact.FrontLeft"))))
	{
		return EHitReactAnim::FrontLeft;
	}

	if (CueTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.HitReact.FrontRight"))))
	{
		return EHitReactAnim::FrontRight;
	}
	
	return EHitReactAnim::Back;
}