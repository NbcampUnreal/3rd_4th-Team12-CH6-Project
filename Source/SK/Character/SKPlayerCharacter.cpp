// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Controller/SKPlayerController.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "Animation/SKPlayerAnimInstance.h"
#include "Component/BattleComponent.h"
#include "Component/SKCombatComponent.h"
#include "GameData/WeaponDataRow.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameState/SKGameState.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Weapon/SKWeaponData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

ASKPlayerCharacter::ASKPlayerCharacter()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 메시 Transform 설정
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->bEnableUpdateRateOptimizations = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	bReplicates = true;


	//틱활성화
	PrimaryActorTick.bCanEverTick = true;

	//모션워핑
	MotionWarpingComp =
		CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComp"));

	// 컴뱃컴포넌트 활성화
	
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));


	// InteractionComponent
	InteractionComponent = CreateDefaultSubobject<USKInteractionComponent>(TEXT("InteractionComponent"));

	// ActionComponent
	ActionComponent = CreateDefaultSubobject<USKActionComponent>(TEXT("ActionComponent"));
}

void ASKPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		HitPPMID =
			UMaterialInstanceDynamic::Create(HitPostProcessMI, this);

		FollowCamera->PostProcessSettings.WeightedBlendables.Array.Add(
			FWeightedBlendable(1.f, HitPPMID)
		);

		ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
		if (!PS)
			return;
		AttributeSet = PS->GetAttributeSet();

		AttributeSet->OnDamageTaken.AddUObject(
			this,
			&ASKPlayerCharacter::OnDamageTaken
		);
	}
	//	SetPlayerStateTag();

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

	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	const FGameplayTag HitTag = TAG_State_Condition_Hit;

	AbilitySystemComponent->RegisterGameplayTagEvent(
		HitTag,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(
		this,
		&ASKPlayerCharacter::OnHitConditionTagChanged
	);
}

void ASKPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled() || !HitPPMID)
		return;
	
	const float Speed = (CurrentHitAlpha < TargetHitAlpha)
		? HitAlphaRiseSpeed     
		: HitAlphaFallSpeed;    

	CurrentHitAlpha = FMath::FInterpTo(CurrentHitAlpha, TargetHitAlpha, DeltaTime, Speed);

	HitPPMID->SetScalarParameterValue(TEXT("Noise Strength"), CurrentHitAlpha);
}

void ASKPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(ASKPlayerCharacter, CurrentWeaponTag);
	// DOREPLIFETIME(ASKPlayerCharacter, ComboState);

	// DOREPLIFETIME(ASKPlayerCharacter, bIsLockedOn);
}

void ASKPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	// if (PS)
	// {
	// 	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	// 	AttributeSet = PS->GetAttributeSet();
	//
	// 	// AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	// }
	SetPlayerStateTag();
	SetTraceSocket();
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	if (PS)
	{
		PS->EquipmentComponentSetting();
	}

	ActionComponent->OnOwnerPossessed();
}

void ASKPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UE_LOG(LogTemp, Warning, TEXT("[CHECK] Mesh=%s"), *GetMesh()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("[CHECK] AnimInstance=%s"),
	       GetMesh()->GetAnimInstance() ? *GetMesh()->GetAnimInstance()->GetName() : TEXT("NULL"));

	if (USkeletalMeshComponent* SKMesh = GetMesh())
	{
		SKMesh->OnAnimInitialized.AddDynamic(this, &ASKPlayerCharacter::OnAnimInitialized);
		UE_LOG(LogTemp, Warning, TEXT("[CHECK] OnAnimInitialized 바인딩 완료"));
	}
}

void ASKPlayerCharacter::SetSprinting(bool bSprinting)
{
	bIsSprinting = bSprinting;
	float WalkSpeed = AttributeSet->GetSpeed();
	float SprintSpeed = AttributeSet->GetSprintWeight() * WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void ASKPlayerCharacter::UpdateMovementTag()
{
	if (!IsValid(AbilitySystemComponent))
		return;

	const float Speed = GetVelocity().Size();
	const bool bIsFalling = GetCharacterMovement()->IsFalling();

	SetLooseTag(TAG_State_Posture_Grounded, !bIsFalling);
	SetLooseTag(TAG_State_Posture_Air, bIsFalling);

	// // Movement 처리
	if (!bIsFalling)
	{
		const bool bIsMoving = (Speed > 10.f);

		SetLooseTag(TAG_State_Movement_Walk, bIsMoving);
		SetLooseTag(TAG_State_Movement_Idle, !bIsMoving);

		// SetLooseTag(TAG_State_Posture_Grounded, !bIsFalling);
	}
	// else
	// {
	// 	SetLooseTag( TAG_State_Posture_Air, bIsFalling);
	// }
}

void ASKPlayerCharacter::UpdateMovementTag_ATK(FGameplayTag ATKTag, bool Enable)
{
	if (!IsValid(AbilitySystemComponent))
		return;

	SetLooseTag(TAG_State_Movement_Walk, false);
	SetLooseTag(TAG_State_Movement_Idle, false);
	SetLooseTag(TAG_State_Movement_Sprint, false);

	//인자로받은 태/비활성화
	SetLooseTag(ATKTag, Enable);
}

void ASKPlayerCharacter::SetTraceSocket()
{
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();

	if (!PS || !PS->GetWeaponDT())
		return;

	const FSKWeaponDataRow* Row = PS->GetWeaponSocketDataRow();
	if (!Row)
		return;

	BattleComponent->InitializeWeaponSocket(Row);

	const FWeaponDataRow* DataRow = PS->GetWeaponDataRow();
	if (!DataRow)
		return;
	BattleComponent->InitializeWeaponData(DataRow);
}


void ASKPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	SetPlayerStateTag();
}


void ASKPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	UpdateMovementTag(); // Idle/Move 상태 갱신 함수
}

void ASKPlayerCharacter::OnHitConditionTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (NewCount > 0)
	{
		// HitCondition 시작
		PC->SetIgnoreMoveInput(true);
		//PC->SetIgnoreLookInput(true); // 선택
	}
	else
	{
		// HitCondition 종료
		PC->SetIgnoreMoveInput(false);
		//PC->SetIgnoreLookInput(false);
	}
}

void ASKPlayerCharacter::TryInitASC()
{
	UE_LOG(LogTemp, Warning, TEXT("[TryInitASC] 타이머 Tick 실행됨"));

	if (!GetMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TryInitASC] Mesh NULL"));
		return;
	}

	if (!GetMesh()->GetAnimInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TryInitASC] AnimInstance NULL"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[TryInitASC] AnimInstance OK"));

	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TryInitASC] PlayerState NULL"));
		return;
	}

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TryInitASC] ASC NULL"));
		return;
	}

	ASC->InitAbilityActorInfo(PS, this);

	UE_LOG(LogTemp, Error, TEXT("[ASC INIT] 성공! AnimInstance=%s"),
	       *GetMesh()->GetAnimInstance()->GetName());

	GetWorld()->GetTimerManager().ClearTimer(InitASCTimerHandle);
}

void ASKPlayerCharacter::OnDamageTaken(float Damage)
{
	TargetHitAlpha = FMath::Clamp(Damage, 0.1f, 5.f);

	GetWorldTimerManager().ClearTimer(HitEffectTimer);
	GetWorldTimerManager().SetTimer(
		HitEffectTimer,
		this,
		&ASKPlayerCharacter::ResetHitEffectTimer,
		0.3f,
		false
	);
}

void ASKPlayerCharacter::ResetHitEffectTimer()
{
	TargetHitAlpha =0.f;
}

void ASKPlayerCharacter::SetLooseTag(const FGameplayTag& Tag, bool bEnable)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetLooseTag] ASC is null, skip: %s"), *Tag.ToString());
		return;
	}

	if (bEnable)
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(Tag))
		{
			AbilitySystemComponent->AddLooseGameplayTag(Tag);
		}
	}
	else
	{
		if (AbilitySystemComponent->HasMatchingGameplayTag(Tag))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(Tag);
		}
	}
}

void ASKPlayerCharacter::AdjustSpringArmDistance(float WheelValue)
{
	if (!CameraBoom)
		return;

	const float ZoomStep = 50.f; // 휠 감도
	const float MinLength = 250.f; // 최소 거리
	const float MaxLength = 600.f; // 최대 거리

	float NewLength = CameraBoom->TargetArmLength - WheelValue * ZoomStep;
	CameraBoom->TargetArmLength = FMath::Clamp(NewLength, MinLength, MaxLength);
}

void ASKPlayerCharacter::SetLockOnRotateMode(bool bLockOn)
{
	// bUseControllerRotationYaw = true;
	//
	// GetCharacterMovement()->bOrientRotationToMovement = false;
	// GetCharacterMovement()->bUseControllerDesiredRotation = true;

	if (bLockOn)
	{
		// Lock On: Movement 기반 회전 금지
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		
	}
	else
	{
		// Lock Off: 다시 Movement 기반 회전 허용
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

void ASKPlayerCharacter::OnRep_LockOn()
{
	if (USKPlayerAnimInstance* Anim =
	Cast<USKPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsLockedOn = bIsLockedOn;
	}

	SetLockOnRotateMode(bIsLockedOn);
	
}

void ASKPlayerCharacter::SetLockOnState(bool bNewState)
{
	if (!HasAuthority())
		return;

	if (bIsLockedOn == bNewState)
		return;

	bIsLockedOn = bNewState;
	
	ApplyLockOnState();
}

void ASKPlayerCharacter::ApplyLockOnState()
{
	if (USKPlayerAnimInstance* Anim =
		Cast<USKPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsLockedOn = bIsLockedOn;
	}

	SetLockOnRotateMode(bIsLockedOn);
}


void ASKPlayerCharacter::SetPlayerStateTag()
{
	// 주기적인 속도 체크를 위한 타이머 (틱 대신 사용)
	GetWorldTimerManager().SetTimer(MovementCheckTimer, this, &ASKPlayerCharacter::UpdateMovementTag, 0.2f, true);
	// 처음엔 Idle 상태 태그 추가
	SetLooseTag(TAG_State_Movement_Idle, true);
}


UBattleComponent* ASKPlayerCharacter::GetBattleComponent() const
{
	return BattleComponent;
}

UMotionWarpingComponent* ASKPlayerCharacter::GetMotionWarpingComponent()
{
	return MotionWarpingComp;
}

void ASKPlayerCharacter::OnAnimInitialized()
{
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	if (!PS)
		return;

	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSet = PS->GetAttributeSet();

	AbilitySystemComponent->InitAbilityActorInfo(PS, this);

	UE_LOG(LogTemp, Error, TEXT("[ASC INIT] AnimInstance Initialized!"));
}
