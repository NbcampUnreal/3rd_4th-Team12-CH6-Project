// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Controller/SKPlayerController.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "Component/SKCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameState/SKGameState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"
#include "GameData/WeaponDataRow.h"
#include "Weapon/SKWeaponData.h"

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
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
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

	// 컴뱃컴포넌트 활성화
	CombatComponent = CreateDefaultSubobject<USKCombatComponent>(TEXT("CombatComponent"));
}

void ASKPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

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

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		FGameplayTag InteractTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.Interact"));
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InteractTag));
	}
}

void ASKPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LockOnTarget(DeltaTime);
}

void ASKPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(ASKPlayerCharacter, CurrentWeaponTag);
	// DOREPLIFETIME(ASKPlayerCharacter, ComboState);
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

	SetLooseTag(AbilitySystemComponent, TAG_State_Posture_Grounded, !bIsFalling);
	SetLooseTag(AbilitySystemComponent, TAG_State_Posture_Air, bIsFalling);

	// Movement 처리
	if (!bIsFalling)
	{
		const bool bIsMoving = (Speed > 10.f);

		SetLooseTag(AbilitySystemComponent, TAG_State_Movement_Walk, bIsMoving);
		SetLooseTag(AbilitySystemComponent, TAG_State_Movement_Idle, !bIsMoving);

		SetLooseTag(AbilitySystemComponent, TAG_State_Posture_Grounded, !bIsFalling);
	}
	else
	{
		SetLooseTag(AbilitySystemComponent, TAG_State_Posture_Air, bIsFalling);
	}
}

void ASKPlayerCharacter::UpdateMovementTag_ATK(FGameplayTag ATKTag, bool Enable)
{
	if (!IsValid(AbilitySystemComponent))
		return;

	SetLooseTag(AbilitySystemComponent, TAG_State_Movement_Walk, false);
	SetLooseTag(AbilitySystemComponent, TAG_State_Movement_Idle, false);
	// SetLooseTag(AbilitySystemComponent, TAG_State_Movement_Walk, false);

	//인자로받은 태그 활성화/비활성화
	SetLooseTag(AbilitySystemComponent, ATKTag, Enable);
}

void ASKPlayerCharacter::SetTraceSocket()
{
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();

	if (!PS || !PS->GetWeaponDT())
		return;

	const FSKWeaponDataRow* Row = PS->GetWeaponSocketDataRow();
	if (!Row)
		return;

	CombatComponent->InitializeWeaponSocket(Row);
	
	const FWeaponDataRow* DataRow = PS->GetWeaponDataRow();
	if (!DataRow)
		return;
	CombatComponent->InitializeWeaponData(DataRow);
}

void ASKPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	SetTraceSocket(); // 안전
}


void ASKPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	UpdateMovementTag(); // Idle/Move 상태 갱신 함수
}

void ASKPlayerCharacter::SetLooseTag(UAbilitySystemComponent* ASC, const FGameplayTag& Tag, bool bEnable)
{
	if (!ASC)
		return;

	if (bEnable)
	{
		if (!ASC->HasMatchingGameplayTag(Tag))
		{
			ASC->AddLooseGameplayTag(Tag);
		}
	}
	else
	{
		if (ASC->HasMatchingGameplayTag(Tag))
		{
			ASC->RemoveLooseGameplayTag(Tag);
		}
	}
}

void ASKPlayerCharacter::LockOnTarget(float DeltaTime)
{
	ASKPlayerController* PC = Cast<ASKPlayerController>(Controller);
	if (PC && PC->bIsLockedOn && PC->CurrentTarget)
	{
		FVector Dir = (PC->CurrentTarget->GetActorLocation() - GetActorLocation());
		Dir.Z = 0;

		FRotator NewRot = FMath::RInterpTo(GetActorRotation(), Dir.Rotation(), DeltaTime, 6.f);
		SetActorRotation(NewRot);
	}
}

void ASKPlayerCharacter::Client_PlayPickupSound_Implementation(USoundBase* PickupSound)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
}

void ASKPlayerCharacter::Server_TryInteract_Implementation(AActor* Target)
{
	if (!Target) return;

	ISKInteractable::Execute_Interact(Target, this);
}

void ASKPlayerCharacter::SetPlayerStateTag()
{
	// 주기적인 속도 체크를 위한 타이머 (틱 대신 사용)
	GetWorldTimerManager().SetTimer(MovementCheckTimer, this, &ASKPlayerCharacter::UpdateMovementTag, 0.2f, true);
	// 처음엔 Idle 상태 태그 추가
	SetLooseTag(AbilitySystemComponent, TAG_State_Movement_Idle, true);
}

USKCombatComponent* ASKPlayerCharacter::GetCombatComponent() const
{
	return CombatComponent;
}
