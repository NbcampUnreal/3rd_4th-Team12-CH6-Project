// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/SKPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Manager/SKCameraManager.h"
#include "Character/SKCharacterBase.h"
#include "Character/SKPlayerCharacter.h"
#include "Character/AI/SKAICharacterBase.h"
#include "Constants/SKGameConstants.h"
#include "Engine/OverlapResult.h"
#include "GameData/SKGameConstant.h"
#include "GameFramework/Character.h"
#include "Utility/SKUIManagerSubSystem.h"
#include "Component/SKCombatComponent.h"
#include "GameInstance/SKGameInstance.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"
#include "PlayerState/SKPlayerState.h"

ASKPlayerController::ASKPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASKPlayerController::ClientShowLoadingScreen_Implementation(bool bShow)
{
	USKGameInstance* SKGI = Cast<USKGameInstance>(GetGameInstance());
	if (SKGI)
	{
		SKGI->ShowLoadingScreen(bShow);
		UE_LOG(LogTemp, Log, TEXT("[PC] ShowLoadingScreen executed. bShow = %s"), bShow ? TEXT("true") : TEXT("false"));
	}
}

void ASKPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		check(DefaultMappingContext);
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ASKPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsLockedOn && CurrentTarget)
	{
		float Dist = FVector::Dist(GetPawn()->GetActorLocation(), CurrentTarget->GetActorLocation());
		if (Dist > LockOnRadius * 1.2f)
		{
			SetLockOnTarget(nullptr);
		}
	}
}

void ASKPlayerController::EnterDungeonByID(int32 DungeonID)
{
	bool bIsHost = (IsLocalController() && GetNetMode() == NM_ListenServer);
	bool bIsSingle = (GetNetMode() == NM_Standalone);
	
	if (!bIsHost && !bIsSingle)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Client] EnterDungeonByID is host-only."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[Host] Request EnterDungeon ID: %d"), DungeonID);
	Server_EnterDungeon(DungeonID);
}

void ASKPlayerController::Server_EnterDungeon_Implementation(int32 DungeonID)
{
	USKGameInstance* GI = GetGameInstance<USKGameInstance>();
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("[Server] GameInstance not found"));
		return;
	}

	GI->TravelToDungeon(DungeonID);
}

void ASKPlayerController::ReturnToTown()
{
	bool bIsHost = (IsLocalController() && GetNetMode() == NM_ListenServer);
	bool bIsSingle = (GetNetMode() == NM_Standalone);
	
	if (!bIsHost && !bIsSingle)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Client] ReturnToTown is host-only."));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("[Host] Request ReturnToTown"));
	Server_ReturnToTown();
}

void ASKPlayerController::Server_ReturnToTown_Implementation()
{	
	USKGameInstance* GI = GetGameInstance<USKGameInstance>();
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("[Server] GameInstance not found"));
		return;
	}

	GI->TravelToTown();
}

void ASKPlayerController::LeaveSessionAndReturnToLocalTown()
{
	auto* GI = GetGameInstance<USKGameInstance>();
	if (!GI) return;

	// ✅ Host → 세션 종료 후 로컬 복귀
	if (HasAuthority() && GetNetMode() != NM_Client)
	{
		UE_LOG(LogTemp, Log, TEXT("[Host] Ending Session → Return to Local Town."));
		GI->LeaveSession();
	}
	else
	{
		// ✅ Client → 네트워크 연결 종료 후 로컬 복귀
		UE_LOG(LogTemp, Log, TEXT("[Client] Disconnecting and returning to local Town."));
		FString TravelCmd = FString::Printf(TEXT("%s"), SKGameConstants::TownLevel);
		ClientTravel(TravelCmd, TRAVEL_Absolute);
	}
}

void ASKPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		check(MoveAction);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                   &ASKPlayerController::StopJumping);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Dash);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this,
		                                   &ASKPlayerController::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
		                                   &ASKPlayerController::StopSprint);

		EnhancedInputComponent->BindAction(LeftAttackAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::LeftAttack);
		EnhancedInputComponent->BindAction(RightAttackAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::RightAttack);
		EnhancedInputComponent->BindAction(MouseWheelAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Active_MouseWheel);
		EnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Interact);
		EnhancedInputComponent->BindAction(QuickSlotAction_00, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Active_QuickSlotAction_00);
		EnhancedInputComponent->BindAction(QuickSlotAction_01, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Active_QuickSlotAction_01);
		EnhancedInputComponent->BindAction(QuickSlotAction_02, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Active_QuickSlotAction_02);
		EnhancedInputComponent->BindAction(QuickSlotItem_00, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Active_QuickSlotItem_00);
		EnhancedInputComponent->BindAction(QuickSlotItem_01, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Active_QuickSlotItem_01);
		EnhancedInputComponent->BindAction(QuickSlotItem_02, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Active_QuickSlotItem_02);
	}
}

void ASKPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("ASKPlayerController::OnPossess"));
	OnPawnPossessed.Broadcast(InPawn);;
}

void ASKPlayerController::Dash(const FInputActionValue& Value)
{
	ASKCharacterBase* SKChar = Cast<ASKCharacterBase>(GetPawn());
	if (!SKChar)
		return;
	UAbilitySystemComponent* ASC = SKChar->GetAbilitySystemComponent();
	if (!ASC)
		return;


	FGameplayTagContainer DashTag;
	DashTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Dash")));

	ASC->TryActivateAbilitiesByTag(DashTag);
}

void ASKPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InMoveVector = Value.Get<FVector2D>();
		CurrentInputVector = InMoveVector;
		CurrentMoveDirection = GetClosestMoveDirection(InMoveVector);
		const FRotator ControlrRotation = GetControlRotation();
		const FRotator ControlYawRotation(0.f, ControlrRotation.Yaw, 0.f);

		const FVector InLookVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
		const FVector InRightVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);


		ControlledPawn->AddMovementInput(InLookVector, InMoveVector.X);
		ControlledPawn->AddMovementInput(InRightVector, InMoveVector.Y);
	}
}

void ASKPlayerController::Look(const FInputActionValue& Value)
{
	if (bIsLockedOn && CurrentTarget)
		return;
	
	const FVector2D InLookVector = Value.Get<FVector2D>();

	AddYawInput(InLookVector.X);
	AddPitchInput(InLookVector.Y);
}

void ASKPlayerController::Jump(const FInputActionValue& Value)
{
	if (ACharacter* pCharacter = Cast<ACharacter>(GetPawn()))
	{
		pCharacter->Jump();
	}
}

void ASKPlayerController::StopJumping()
{
	if (ACharacter* pCharacter = Cast<ACharacter>(GetPawn()))
	{
		pCharacter->StopJumping();
	}
}

void ASKPlayerController::StartSprint(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
		return;

	FGameplayTag SprintTag = FGameplayTag::RequestGameplayTag(FName("Ability.Sprint"));
	FGameplayTagContainer SprintTagContainer;
	SprintTagContainer.AddTag(SprintTag);

	ASC->TryActivateAbilitiesByTag(SprintTagContainer);
}

void ASKPlayerController::StopSprint(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
		return;

	FGameplayTagContainer SprintTagContainer;
	SprintTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Sprint")));

	ASC->CancelAbilities(&SprintTagContainer);
}

void ASKPlayerController::LeftAttack(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	USKCombatComponent* CombatComponent = PlayerCharacter->GetCombatComponent();
	CombatComponent->Server_LeftAttackInput();
}

void ASKPlayerController::RightAttack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("RIGHT"));
}

void ASKPlayerController::Active_MouseWheel(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("ACTIVE_MOUSE_WHEEL"));

	if (bIsLockedOn)
	{
		SetLockOnTarget(nullptr);
		return;
	}

	AActor* Target = FindNearestTarget();
	if (Target)
	{
		SetLockOnTarget(Target);
	}
}

void ASKPlayerController::Active_QuickSlotAction_00(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotAction_00"));
}

void ASKPlayerController::Active_QuickSlotAction_01(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotAction_01"));
}

void ASKPlayerController::Active_QuickSlotAction_02(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotAction_02"));
}

void ASKPlayerController::Active_QuickSlotItem_00(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotItem_00"));
}

void ASKPlayerController::Active_QuickSlotItem_01(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotItem_01"));
}

void ASKPlayerController::Active_QuickSlotItem_02(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotItem_02"));
}

AActor* ASKPlayerController::FindNearestTarget()
{
	APawn* thisPlayer = GetPawn();
	if (!thisPlayer)
		return nullptr;
	
	FVector Origin = thisPlayer->GetActorLocation();
	TArray<FOverlapResult> Overlaps;
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(LockOnRadius);
	
	bool bHit = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Origin,
		FQuat::Identity,
		ECC_Pawn,
		Sphere
	);
	
	if (!bHit)
		return nullptr;
	
	float MinDist = FLT_MAX;
	AActor* Best = nullptr;
	
	for (auto& Result : Overlaps)
	{
		AActor* A = Result.GetActor();
		if (!A || A == thisPlayer) continue;

		ASKAICharacterBase* AI = Cast<ASKAICharacterBase>(A);
		if (!AI)
			continue; //AI만찾기
		
		float D = FVector::Dist(Origin, A->GetActorLocation());
		if (D < MinDist)
		{
			MinDist = D;
			Best = A;
		}
	}

	return Best;
}

void ASKPlayerController::SetLockOnTarget(AActor* NewTarget)
{
	CurrentTarget = NewTarget;
	bIsLockedOn = (NewTarget != nullptr);

	UpdateCameraManagerTarget();
}

void ASKPlayerController::UpdateCameraManagerTarget()
{
	ASKCameraManager* Cam = Cast<ASKCameraManager>(PlayerCameraManager);
	if (Cam)
	{
		Cam->LockedTarget = CurrentTarget;
		Cam->bIsLockedOn = bIsLockedOn;
	}
}

EMoveDirection ASKPlayerController::GetClosestMoveDirection(const FVector2D& InputVector)
{
	if (InputVector.IsNearlyZero())
	{
		return EMoveDirection::None;
	}
 
	FVector2D NormalizedInput = InputVector.GetSafeNormal();
 
	const FVector2D Forward(1.f, 0.f);   // X+
	const FVector2D Backward(-1.f, 0.f); // X-
	const FVector2D Right(0.f, 1.f);     // Y+
	const FVector2D Left(0.f, -1.f);     // Y-

 
	float Dots[4];
	Dots[0] = FVector2D::DotProduct(NormalizedInput, Forward);
	Dots[1] = FVector2D::DotProduct(NormalizedInput, Backward);
	Dots[2] = FVector2D::DotProduct(NormalizedInput, Left);
	Dots[3] = FVector2D::DotProduct(NormalizedInput, Right);
 
	// 최대 Dot 값 가진 방향 찾기
	float MaxDot = -1.0f;
	EMoveDirection BestDirection = EMoveDirection::None;
 
	for (int i = 0; i < 4; ++i)
	{
		if (Dots[i] > MaxDot)
		{
			MaxDot = Dots[i];

			switch (i)
			{
				case 0: BestDirection = EMoveDirection::Forward; break;
				case 1: BestDirection = EMoveDirection::Backward; break;
				case 2: BestDirection = EMoveDirection::Left; break;
				case 3: BestDirection = EMoveDirection::Right; break;
			}
		}
	}
 
	return BestDirection;
}


void ASKPlayerController::Interact(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Interact"));

	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetPawn());
	if (!SKPlayerCharacter) return;
	
	USKInteractionComponent* InteractionComponent = SKPlayerCharacter->GetInteractionComponent();
	if (InteractionComponent)
	{
		InteractionComponent->Server_TryInteract();
	}
	
};
