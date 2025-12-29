// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/HitReact/GC_HitReact.h"
#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Utility/HitReactUtils.h"
//#include "GameplayTagContainer.h"


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

	
	// =========================
	// 1️⃣ 방향 계산 (항상 수행)
	// =========================
	FVector KnockBackDir = FVector::BackwardVector;
	EHitReactAnim ReactType = EHitReactAnim::Back;
	AActor* Instigator = nullptr;

	if (Parameters.EffectContext.IsValid())
	{
		Instigator = Parameters.EffectContext.GetOriginalInstigator();
		if (Instigator)
		{
			KnockBackDir = (Character->GetActorLocation() - Instigator->GetActorLocation()).GetSafeNormal();
			ReactType = DetermineHitReactAnim(Instigator->GetActorLocation(), Character);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HitReact: Failed to determine reaction type"));
			return false;
		}
	}

	// =========================
	// 2️⃣ HitReact 타입 판별
	// =========================
	const FGameplayTag& CueTag = Parameters.OriginalTag;

	// Heavy
	if (CueTag.MatchesTag(TAG_GameplayCue_HitReact_Heavy))
	{
		PlayHeavy(Character, Instigator, WeaponAnimData, KnockBackDir, ReactType);
		return true;
	}

	// Unblockable
	if (CueTag.MatchesTag(TAG_GameplayCue_HitReact_Unblockable))
	{
		PlayUnblockable(Character, Instigator, WeaponAnimData, KnockBackDir, ReactType);
		return true;
	}

	PlayNormal(Character, Instigator, WeaponAnimData, KnockBackDir, ReactType);

	return true;
}

void UGC_HitReact::PlayNormal(
	ASKPlayerCharacter* Character,
	AActor* Enemy,
	USKWeaponAnimData* AnimData,
	const FVector& KnockBackDir,
	const EHitReactAnim ReactType
) const
{
	UAnimMontage* Montage = nullptr;

	switch (ReactType)
	{
	case EHitReactAnim::FrontLeft:
		UE_LOG(LogTemp, Warning, TEXT("PlayNormal: FrontLeft Anim"));
		Montage = AnimData->HitReactMontages.FrontLeft;
		break;

	case EHitReactAnim::FrontRight:
		UE_LOG(LogTemp, Warning, TEXT("PlayNormal: FrontRight Anim"));
		Montage = AnimData->HitReactMontages.FrontRight;
		break;

	case EHitReactAnim::Back:
		UE_LOG(LogTemp, Warning, TEXT("PlayNormal: Back Anim"));
		Montage = AnimData->HitReactMontages.Back;
		break;
	}

	if (Montage)
	{
		Character->PlayAnimMontage(Montage);
	}
}

void UGC_HitReact::PlayHeavy(
	ASKPlayerCharacter* Character,
	AActor* Enemy,
	USKWeaponAnimData* AnimData,
	const FVector& KnockBackDir,
	const EHitReactAnim ReactType
) const
{
	if (!AnimData->HitReactMontages.Heavy) return;

	// 공격 방향으로 회전
	FVector LookAtDir = (Enemy->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
	FRotator LookAtRot = LookAtDir.Rotation();
	LookAtRot.Pitch = 0.f;
	LookAtRot.Roll  = 0.f;
	
	Character->bUseControllerRotationYaw = false;
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	
	Character->SetActorRotation(LookAtRot);

	if (EHitReactAnim::Back == ReactType)
	{
		//뒤에서 맞았을 경우 날라가는 몽타주 재생
		Character->PlayAnimMontage(AnimData->HitReactMontages.Unblockable);
	}
	else
	{
		//앞에서 맞았을 경우 밀려나는 몽타주 재생
		Character->PlayAnimMontage(AnimData->HitReactMontages.Heavy);
	}
	
}

void UGC_HitReact::PlayUnblockable(
	ASKPlayerCharacter* Character,
	AActor* Enemy,
	USKWeaponAnimData* AnimData,
	const FVector& KnockBackDir,
	const EHitReactAnim ReactType
) const
{
	if (!AnimData->HitReactMontages.Unblockable) return;

	// 공격 방향으로 회전
	FVector LookAtDir = (Enemy->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
	FRotator LookAtRot = LookAtDir.Rotation();
	LookAtRot.Pitch = 0.f;
	LookAtRot.Roll  = 0.f;
	
	Character->bUseControllerRotationYaw = false;
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->SetActorRotation(LookAtRot);

	Character->PlayAnimMontage(AnimData->HitReactMontages.Unblockable);
}