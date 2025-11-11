// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Controller/SKPlayerController.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameState/SKGameState.h"
#include "Interaction/Interface/IInteractable.h"

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
	SetPlayerStateTag();
	
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

void ASKPlayerCharacter::SetSprinting(bool bSprinting)
{
	bIsSprinting = bSprinting;
	float WalkSpeed=	AttributeSet->GetSpeed();
	float SprintSpeed=	AttributeSet->GetSprintWeight() * WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void ASKPlayerCharacter::UpdateMovementTag()
{
	if (!AbilitySystemComponent)
		return;

	const float Speed = GetVelocity().Size();
	const FGameplayTag IdleTag = FGameplayTag::RequestGameplayTag(TEXT("PlayerState.Idle"));
	const FGameplayTag MoveTag = FGameplayTag::RequestGameplayTag(TEXT("PlayerState.Move"));


	if (Speed > 5.f)
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(MoveTag))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(IdleTag);
			AbilitySystemComponent->AddLooseGameplayTag(MoveTag);
		}
	}
	else
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(IdleTag))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(MoveTag);
			AbilitySystemComponent->AddLooseGameplayTag(IdleTag);
		}
	}
}

void ASKPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	UpdateMovementTag(); // Idle/Move 상태 갱신 함수
}

void ASKPlayerCharacter::SetPlayerStateTag()
{
	// 주기적인 속도 체크를 위한 타이머 (틱 대신 사용)
	GetWorldTimerManager().SetTimer(MovementCheckTimer, this, &ASKPlayerCharacter::UpdateMovementTag, 0.2f, true);

	// 처음엔 Idle 상태 태그 추가
	AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("PlayerState.Idle")));
}

void ASKPlayerCharacter::TraceForInteraction()
{
	UCameraComponent* CameraComponent = GetFollowCamera();

	FVector Start = GetActorLocation();
	
	FVector Direction = CameraComponent->GetForwardVector();
	Direction.Z = 0.f;
	Direction.Normalize();

	FVector End = Start + Direction * 200.0f;

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FInteractionData InteractionData;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			IInteractable::Execute_GetInteractionData(HitActor, InteractionData);
			IInteractable::Execute_Interact(HitActor, this);
		}
	}
	if (!AbilitySystemComponent) return;
	
	FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(
		FGameplayAbilitySpec(InteractionData.GrantedAbility, 1, INDEX_NONE, this)
	);
	AbilitySystemComponent->TryActivateAbility(Handle);
}


