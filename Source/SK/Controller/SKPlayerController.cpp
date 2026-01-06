// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/SKPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NNETypes.h"
#include "Animation/SKPlayerAnimInstance.h"
#include "Manager/SKCameraManager.h"
#include "Character/SKCharacterBase.h"
#include "Character/SKPlayerCharacter.h"
#include "Character/AI/SKAICharacterBase.h"
#include "Component/QuickSlotComponent.h"
#include "Constants/SKGameConstants.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "Component/BattleComponent.h"
#include "GameInstance/SKGameInstance.h"
#include "GameMode/SKGameMode.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"
#include "Item/Bonfire/SKBonfire.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

ASKPlayerController::ASKPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerCameraManagerClass = ASKCameraManager::StaticClass();
}

void ASKPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASKPlayerController, bIsLockedOn);
	DOREPLIFETIME(ASKPlayerController, LockedTarget);
	DOREPLIFETIME(ASKPlayerController, bCanMaintainCombo);
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

void ASKPlayerController::Server_SetControlRotation_Implementation(const FRotator& NewRotation)
{
	if (APawn* PlayerPawn = GetPawn())
	{
		PlayerPawn->Controller->SetControlRotation(NewRotation);
	}
}

void ASKPlayerController::Server_SetLockedTarget_Implementation(AActor* NewTarget)
{
	SetLockedTarget(NewTarget);
}

void ASKPlayerController::Server_SetLockOnState_Implementation(bool bNewState)
{
	SetLockOnState(bNewState);
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


	if (bIsLockedOn)
	{
		if (!ValidateLockOn())
		{
			// 예외시 Lock OFF
			ResetLockOn();
			return;
		}

		if (LockedTarget)
		{
			UpdateLockOnRotation(DeltaTime);
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

void ASKPlayerController::ResetLockOn()
{
	SetLockedTarget(nullptr);
	SetLockOnState(false);
}


bool ASKPlayerController::GetIsLockedOn()
{
	return bIsLockedOn;
}

void ASKPlayerController::SetIsLockedOn(bool ArgIsLockedOn)
{
	bIsLockedOn = ArgIsLockedOn;
}

AActor* ASKPlayerController::GetLockedTarget()
{
	return LockedTarget;
}

void ASKPlayerController::SetLockOnState(bool bNewState)
{
	if (HasAuthority())
	{
		bIsLockedOn = bNewState;
		// OnRep_LockOnChanged();

		if (ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetPawn()))
		{
			PlayerCharacter->SetLockOnState(bNewState);
		}
	}
	else
	{
		Server_SetLockOnState(bNewState);
	}
}

void ASKPlayerController::SetLockedTarget(AActor* NewTarget)
{
	if (HasAuthority())
	{
		LockedTarget = NewTarget;
		OnRep_LockedTargetChanged();
	}
	else
	{
		Server_SetLockedTarget(NewTarget);
	}
}


void ASKPlayerController::OnRep_LockOnChanged()
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetPawn());
	if (!SKPlayer)
		return;
	
	if (USKPlayerAnimInstance* Anim =
	Cast<USKPlayerAnimInstance>(SKPlayer->GetMesh()->GetAnimInstance()))
	{
		Anim->bIsLockedOn = bIsLockedOn;
	}
	
	SKPlayer->SetLockOnRotateMode(bIsLockedOn);
}

void ASKPlayerController::OnRep_LockedTargetChanged()
{
	if (OldTarget)
	{
		if (ASKAICharacterBase* AI = Cast<ASKAICharacterBase>(OldTarget))
			AI->ClearOverlayMaterial();
	}

	if (LockedTarget)
	{
		ASKCameraManager* Cam = Cast<ASKCameraManager>(PlayerCameraManager);

		if (IsValid(Cam))
		{
			if (ASKAICharacterBase* AI = Cast<ASKAICharacterBase>(LockedTarget))
				AI->SetOverlayMaterial(Cam->Get_OutLineMat(), Cam->Get_OutLineTime());
		}
	}
	OldTarget = LockedTarget;
}

bool ASKPlayerController::ValidateLockOn()
{
	if (!LockedTarget)
		return false;
	if (LockedTarget->IsActorBeingDestroyed())
		return false;

	APawn* PlayerPawn = GetPawn();
	if (!PlayerPawn)
		return false;

	float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), LockedTarget->GetActorLocation());

	ASKCameraManager* Cam = Cast<ASKCameraManager>(PlayerCameraManager);
	if (Dist > Cam->MaxLockDistance || Dist < Cam->MinLockDistance)
		return false;


	return true;
}

void ASKPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		check(MoveAction);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this,
		                                   &ASKPlayerController::OnMoveRepleased);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                   &ASKPlayerController::StopJumping);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Dash);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
		                                   &ASKPlayerController::StopSprint);

		EnhancedInputComponent->BindAction(LeftAttackAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::LeftAttack);
		EnhancedInputComponent->BindAction(RightAttackAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::RightAttack);
		EnhancedInputComponent->BindAction(MouseWheelAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Active_MouseWheel);
		EnhancedInputComponent->BindAction(MouseWheelMoveAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Active_MouseWheelMove);

		EnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Interact);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::Dodge);
		// Q-R-F
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
		//가드
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Started, this,
		                                   &ASKPlayerController::StartGuard);
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Completed, this,
		                                   &ASKPlayerController::StopGuard);
	}
}

void ASKPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("ASKPlayerController::OnPossess"));
	OnPawnPossessed.Broadcast(InPawn);;
}


void ASKPlayerController::UpdateLockOnRotation(float DeltaTime)
{
	if (!IsValid(LockedTarget))
	{
		return;
	}

	APawn* PlayerPawn = GetPawn();
	if (!IsValid(PlayerPawn))
	{
		return;
	}
	FVector PlayerLoc = PlayerPawn->GetActorLocation();
	FVector TargetLoc = LockedTarget->GetActorLocation();

	ASKCameraManager* Cam = Cast<ASKCameraManager>(PlayerCameraManager);
	TargetLoc.Z += Cam->LockOnHeight;

	FRotator TargetRot = (TargetLoc - PlayerLoc).Rotation();
	TargetRot.Pitch -= Cam->LockOnPitch;

	Server_SetFacingDirection(TargetRot.Yaw);

	//록온시 카메라 회전폐기
	// FRotator NewRot = FMath::RInterpTo(GetControlRotation(), TargetRot, DeltaTime, Cam->LockOnInterpSpeed);
	//
	// SetControlRotation(NewRot);
}

void ASKPlayerController::Server_SetFacingDirection_Implementation(float NewYaw)
{
	APawn* PlayerPawn = GetPawn();
	if (!IsValid(PlayerPawn))
		return;

	FRotator NewRot = PlayerPawn->GetActorRotation();

	NewRot.Yaw = NewYaw;

	PlayerPawn->SetActorRotation(NewRot);
}

void ASKPlayerController::Dash(const FInputActionValue& Value)
{
	// ASKCharacterBase* SKChar = Cast<ASKCharacterBase>(GetPawn());
	// if (!SKChar)
	// 	return;
	// UAbilitySystemComponent* ASC = SKChar->GetAbilitySystemComponent();
	// if (!ASC)
	// 	return;
	//
	//
	// FGameplayTagContainer DashTag;
	// DashTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Dash")));
	//
	// ASC->TryActivateAbilitiesByTag(DashTag);


}

void ASKPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InMoveVector = Value.Get<FVector2D>();
		const FRotator ControlrRotation = GetControlRotation();
		const FRotator ControlYawRotation(0.f, ControlrRotation.Yaw, 0.f);

		const FVector InLookVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
		const FVector InRightVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);


		ControlledPawn->AddMovementInput(InLookVector, InMoveVector.X);
		ControlledPawn->AddMovementInput(InRightVector, InMoveVector.Y);


		ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
		if (PlayerCharacter)
		{
			USKActionComponent* ActionComponent = PlayerCharacter->GetActionComponent();
			if (ActionComponent)
			{
				const EMoveDirection MoveDirection = GetClosestMoveDirection(InMoveVector);
				ActionComponent->Server_SetMovementInfo(InMoveVector, MoveDirection);
			}
		}
	}
}

void ASKPlayerController::OnMoveRepleased()
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(GetPawn());
	if (PlayerCharacter)
	{
		USKActionComponent* ActionComponent = PlayerCharacter->GetActionComponent();
		if (ActionComponent)
		{
			ActionComponent->Server_SetMovementInfo(FVector2D::ZeroVector,
			                                        GetClosestMoveDirection(FVector2D::ZeroVector));
		}
	}
}

void ASKPlayerController::Look(const FInputActionValue& Value)
{
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

	if (!bSprintFlag)
	{
		bSprintFlag = true;
		PlayerCharacter->SetLooseTag(TAG_State_Movement_Sprint, true);
		PlayerCharacter->SetLooseTag(TAG_State_Movement_Idle, false);
	}
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

	if (bSprintFlag)
	{
		bSprintFlag = false;
		PlayerCharacter->SetLooseTag(TAG_State_Movement_Sprint, false);
		PlayerCharacter->SetLooseTag(TAG_State_Movement_Idle, true);
	}
}

void ASKPlayerController::LeftAttack(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
	{
		return;
	}

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		return;
	}

	FGameplayTagContainer LeftTagContainer;
	LeftTagContainer.AddTag(TAG_Ability_LeftATK);
	LeftTagContainer.AddTag(TAG_Ability_RightATK);

	if (bCanMaintainCombo)
	{
		ServerCancelAbility(ASC, LeftTagContainer);
	}

	FGameplayTagContainer GuardCounterTag;
	GuardCounterTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.GuardCounter")));

	if (ASC->TryActivateAbilitiesByTag(GuardCounterTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("GuardCounter Success"));
	}
	else if (ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(TAG_Ability_LeftATK)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Left Attack Success"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Left Key Fail"));
	}

	bCanMaintainCombo = false;
}

void ASKPlayerController::RightAttack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("RIGHT"));
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
	{
		return;
	}

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		return;
	}

	FGameplayTagContainer RightTagContainer;
	RightTagContainer.AddTag(TAG_Ability_LeftATK);
	RightTagContainer.AddTag(TAG_Ability_RightATK);

	if (bCanMaintainCombo)
	{
		ServerCancelAbility(ASC, RightTagContainer);
	}

	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(TAG_Ability_RightATK));

	bCanMaintainCombo = false;
}

void ASKPlayerController::Active_MouseWheel(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("ACTIVE_MOUSE_WHEEL"));

	if (bIsLockedOn)
	{
		SetLockedTarget(nullptr);
		SetLockOnState(false);
		return;
	}

	AActor* Target = FindNearestTarget();
	SetLockedTarget(Target);
	SetLockOnState(Target != nullptr);

	// 무기 어태치
	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetPawn());
	if (!SKPlayerCharacter) return;

	USKActionComponent* ActionComponent = SKPlayerCharacter->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->OnCombatAction(true);
	}
}

void ASKPlayerController::Active_MouseWheelMove(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("ACTIVE_MOUSE_WHEELUP"));

	float WheelValue = Value.Get<float>(); // 위로 돌리면 +1, 아래 -1

	ASKCameraManager* CamManager = Cast<ASKCameraManager>(PlayerCameraManager);
	if (!CamManager)
		return;

	CamManager->AdjustCameraDistance(WheelValue);

	if (ASKPlayerCharacter* PlayerChar = Cast<ASKPlayerCharacter>(GetPawn()))
	{
		PlayerChar->AdjustSpringArmDistance(WheelValue);
	}
}


void ASKPlayerController::Active_QuickSlotAction_00(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotAction_00"));
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	UBattleComponent* BattleComponent = PlayerCharacter->GetBattleComponent();
	BattleComponent->Server_Input_Skill_01();
}

void ASKPlayerController::Active_QuickSlotAction_01(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotAction_01"));

	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	UBattleComponent* BattleComponent = PlayerCharacter->GetBattleComponent();
	BattleComponent->Server_Input_Skill_02();
}

void ASKPlayerController::Active_QuickSlotAction_02(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotAction_02"));

	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	UBattleComponent* BattleComponent = PlayerCharacter->GetBattleComponent();
	BattleComponent->Server_Input_Skill_03();
}

void ASKPlayerController::Active_QuickSlotItem_00(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotItem_00"));
	APlayerState* BasePlayerState = PlayerState; // 또는 GetPlayerState()

	// 2. 커스텀 PlayerState로 캐스팅
	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(BasePlayerState);
	if (!SKPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState is not of type ASKPlayerState!"));
		return;
	}

	// 3. QuickSlotComponent 찾기
	UQuickSlotComponent* QuickSlotComp = SKPlayerState->FindComponentByClass<UQuickSlotComponent>();
	if (!QuickSlotComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickSlotComponent not found on PlayerState!"));
		return;
	}

	// 4. TryUseQuickSlot 호출 (슬롯 인덱스: 0)
	QuickSlotComp->TryUseQuickSlot(0);
}

void ASKPlayerController::Active_QuickSlotItem_01(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotItem_01"));
	APlayerState* BasePlayerState = PlayerState; // 또는 GetPlayerState()

	// 2. 커스텀 PlayerState로 캐스팅
	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(BasePlayerState);
	if (!SKPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState is not of type ASKPlayerState!"));
		return;
	}

	// 3. QuickSlotComponent 찾기
	UQuickSlotComponent* QuickSlotComp = SKPlayerState->FindComponentByClass<UQuickSlotComponent>();
	if (!QuickSlotComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickSlotComponent not found on PlayerState!"));
		return;
	}

	// 4. TryUseQuickSlot 호출 (슬롯 인덱스: 0)
	QuickSlotComp->TryUseQuickSlot(1);
}

void ASKPlayerController::Active_QuickSlotItem_02(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Active_QuickSlotItem_02"));
	APlayerState* BasePlayerState = PlayerState; // 또는 GetPlayerState()

	// 2. 커스텀 PlayerState로 캐스팅
	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(BasePlayerState);
	if (!SKPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState is not of type ASKPlayerState!"));
		return;
	}

	// 3. QuickSlotComponent 찾기
	UQuickSlotComponent* QuickSlotComp = SKPlayerState->FindComponentByClass<UQuickSlotComponent>();
	if (!QuickSlotComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickSlotComponent not found on PlayerState!"));
		return;
	}

	// 4. TryUseQuickSlot 호출 (슬롯 인덱스: 0)
	QuickSlotComp->TryUseQuickSlot(2);
}

void ASKPlayerController::StartGuard(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("[Controller] StartGuard"));
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
		return;


	FGameplayTagContainer GuardTag;
	GuardTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Guard")));

	ASC->TryActivateAbilitiesByTag(GuardTag);
}

void ASKPlayerController::StopGuard(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("[Controller] StopGuard"));
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
		return;

	FGameplayTagContainer GuardTag;
	GuardTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Guard")));

	ASC->CancelAbilities(&GuardTag);
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


void ASKPlayerController::ClearTarGetOverlayMaterial()
{
	if (!IsValid(LockedTarget))
		return;
	Cast<ASKAICharacterBase>(LockedTarget)->ClearOverlayMaterial();
}


EMoveDirection ASKPlayerController::GetClosestMoveDirection(const FVector2D& InputVector)
{
	if (InputVector.IsNearlyZero())
	{
		return EMoveDirection::None;
	}

	FVector2D NormalizedInput = InputVector.GetSafeNormal();

	const FVector2D Forward(1.f, 0.f);
	const FVector2D ForwardLeft(1.f, -1.f);
	const FVector2D ForwardRight(1.f, 1.f);
	const FVector2D Backward(-1.f, 0.f);
	const FVector2D BackwardLeft(-1.f, -1.f);
	const FVector2D BackwardRight(-1.f, 1.f);
	const FVector2D Right(0.f, 1.f);
	const FVector2D Left(0.f, -1.f);

	float Dots[8];
	Dots[0] = FVector2D::DotProduct(NormalizedInput, Forward);
	Dots[1] = FVector2D::DotProduct(NormalizedInput, ForwardLeft.GetSafeNormal());
	Dots[2] = FVector2D::DotProduct(NormalizedInput, ForwardRight.GetSafeNormal());
	Dots[3] = FVector2D::DotProduct(NormalizedInput, Backward);
	Dots[4] = FVector2D::DotProduct(NormalizedInput, BackwardLeft.GetSafeNormal());
	Dots[5] = FVector2D::DotProduct(NormalizedInput, BackwardRight.GetSafeNormal());
	Dots[6] = FVector2D::DotProduct(NormalizedInput, Left);
	Dots[7] = FVector2D::DotProduct(NormalizedInput, Right);

	// 최대 Dot 값 가진 방향 찾기
	float MaxDot = -1.0f;
	EMoveDirection BestDirection = EMoveDirection::None;

	for (int i = 0; i < 8; ++i)
	{
		if (Dots[i] > MaxDot)
		{
			MaxDot = Dots[i];

			switch (i)
			{
			case 0: BestDirection = EMoveDirection::Forward;
				break;
			case 1: BestDirection = EMoveDirection::ForwardLeft;
				break;
			case 2: BestDirection = EMoveDirection::ForwardRight;
				break;
			case 3: BestDirection = EMoveDirection::Backward;
				break;
			case 4: BestDirection = EMoveDirection::BackwardLeft;
				break;
			case 5: BestDirection = EMoveDirection::BackwardRight;
				break;
			case 6: BestDirection = EMoveDirection::Left;
				break;
			case 7: BestDirection = EMoveDirection::Right;
				break;
			}
		}
	}

	return BestDirection;
}

void ASKPlayerController::RequestRespawn()
{
	if (!HasAuthority()) return;

	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	if (!PS || !PS->CurrentBonfire) return;

	ASKBonfire* Bonfire = Cast<ASKBonfire>(PS->CurrentBonfire);
	FSKInteractionData Data;
	ISKInteractable::Execute_GetInteractionData(Bonfire, Data);

	FTransform SpawnTransform(Data.InteractionRotation, Data.InteractionLocation);

	if (APawn* P = GetPawn())
	{
		P->Destroy();
	}

	AGameModeBase* GM = GetWorld()->GetAuthGameMode();
	if (!GM) return;

	// 리스폰
	GM->RestartPlayerAtTransform(this, SpawnTransform);
	Bonfire->Multicast_SpawnEffect(SpawnTransform);

	// 체력, 스테미나 초기화
	Bonfire->ResetBonfire(Cast<ASKPlayerCharacter>(GetPawn()));

	// 죽음 태그 제거
	UAbilitySystemComponent* ASC = PS->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC) return;

	ASC->RemoveLooseGameplayTag(TAG_State_Condition_Death);
	// 락온 해제
	if (bIsLockedOn)
	{
		SetLockedTarget(nullptr);
		SetLockOnState(false);
	}
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

void ASKPlayerController::Dodge(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Dodge"));

	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetPawn());
	if (!SKPlayerCharacter) return;

	USKActionComponent* ActionComponent = SKPlayerCharacter->GetActionComponent();
	if (ActionComponent)
	{
		ActionComponent->TryDodge();
	}
};

void ASKPlayerController::RequestLevelUp()
{
	if (IsLocalController())
	{
		ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
		if (!PS) return;

		// 클라 → 서버로 요청
		PS->Server_RequestLevelUp();
	}
}

void ASKPlayerController::ServerCancelAbility_Implementation(UAbilitySystemComponent* ASC,
                                                             FGameplayTagContainer CancelAbilityTags)
{
	ASC->CancelAbilities(&CancelAbilityTags, nullptr);
}

bool ASKPlayerController::ServerCancelAbility_Validate(UAbilitySystemComponent* ASC,
                                                       FGameplayTagContainer CancelAbilityTags)
{
	return true;
}
