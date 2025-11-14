// Fill out your copyright notice in the Description page of Project Settings.

#include "Object/SKMeleeHitBox.h"
#include "Anim/SK_GCN_MeleeAttack.h"
#include "GameFramework/Character.h"
#include "SK_GCN_MeleeAttack.h"

ASK_GCN_MeleeAttack::ASK_GCN_MeleeAttack()
{
	bAutoDestroyOnRemove = true;
}

// void ASK_GCN_MeleeAttack::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
// {
// 	Super::OnExecute_Implementation(MyTarget, Parameters);
// }

// void ASK_GCN_MeleeAttack::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
// {
// 	if (!MyTarget || !HitboxClass)
// 		return;
//
// 	ACharacter* Character = Cast<ACharacter>(MyTarget);
// 	if (!Character)
// 		return;
//
// 	// 캐릭터 앞에서 스폰
// 	FVector Forward = Character->GetActorForwardVector();
// 	FVector SpawnLocation = Character->GetActorLocation() + Forward * 100.f;
//
// 	FTransform SpawnTransform;
// 	SpawnTransform.SetLocation(SpawnLocation);
// 	SpawnTransform.SetRotation(Character->GetActorRotation().Quaternion());
//
// 	ASKMeleeHitBox* Hitbox = Character->GetWorld()->SpawnActorDeferred<ASKMeleeHitBox>(
// 		HitboxClass,
// 		SpawnTransform,
// 		Character
// 	);
//
// 	if (Hitbox)
// 	{
// 		Hitbox->InitHitbox(Character, Parameters);
// 		Hitbox->FinishSpawning(SpawnTransform);
// 	}
// 	
// }
