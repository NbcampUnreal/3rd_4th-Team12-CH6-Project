// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Controller/SKPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameState/SKGameState.h"

ASKPlayerCharacter::ASKPlayerCharacter()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// 메시 Transform 설정
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = false;
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	bReplicates = true;
}

void ASKPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetDAPlayerStat();
	
	if (AController* PC = GetController())
	{
		ASKPlayerController* MyPC = Cast<ASKPlayerController>(PC);
		if (MyPC)
		{
			if (ASKGameState* GS = GetWorld()->GetGameState<ASKGameState>())
			{
				// // 기존 바인딩 제거 후 다시 바인딩 (중복 방지)
				// GS->OnMachInProgress.RemoveDynamic(MyPC, &ASKPlayerController::HandleMatchInProgress);
				// GS->OnMatchWaitingPostMatch.RemoveDynamic(MyPC, &ASKPlayerController::HandleMatchWaitingPostMatch);
				//
				// // 컨트롤러 함수 바인딩
				// GS->OnMachInProgress.AddDynamic(MyPC, &ASKPlayerController::HandleMatchInProgress);
				// GS->OnMatchWaitingPostMatch.AddDynamic(MyPC, &ASKPlayerController::HandleMatchWaitingPostMatch);
			}
		}
	}
}
