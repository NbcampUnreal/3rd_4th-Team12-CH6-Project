// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotifyState.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/SKBGMSubSystem.h"

void USKAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	ElapsedTime = 0.f;
	
}

void USKAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (!MeshComp)
		return;

	ACharacter* Owner = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Owner)
		return;

	// 🔹 로컬 플레이어만
	if (!Owner->IsLocallyControlled())
		return;

	UCharacterMovementComponent* MoveComp = Owner->GetCharacterMovement();
	if (!MoveComp)
		return;

	const float Speed = MoveComp->Velocity.Size2D();
	if (Speed < MinMoveSpeed)
		return;

	ElapsedTime += FrameDeltaTime;

	if (ElapsedTime < StepInterval)
		return;

	ElapsedTime = 0.f;

	// 🔹 사운드 서브시스템 호출
	if (UGameInstance* GI = Owner->GetGameInstance())
	{
		if (USKBGMSubSystem* SoundSub = GI->GetSubsystem<USKBGMSubSystem>())
		{
			SoundSub->PlaySoundByTag(
				FootStepSoundTag,
				Owner->GetActorLocation()
			);
		}
	}
	
}

void USKAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	ElapsedTime = 0.f;
}
