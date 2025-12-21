// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_CameraShake.h"

#include "Character/SKPlayerCharacter.h"
#include "Controller/SKPlayerController.h"
#include "Manager/SKCameraManager.h"

void USKAnimNotify_CameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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


	ASKCameraManager* CamManager =
		Cast<ASKCameraManager>(Controller->PlayerCameraManager);
	if (!IsValid(CamManager))
		return;


	CamManager->PlayAttackCameraShake(
		HorizonShakeScale,    // 좌우
		VerticalShakeScale,    // 상하
		Shake_Frequency,   // 빈도
		Shake_Duration   // 지속시간
	);

	
	
}
