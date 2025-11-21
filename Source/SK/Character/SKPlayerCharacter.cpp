// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Controller/SKPlayerController.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "GameData/SKGameConstant.h"
#include "GameData/WeaponDataRow.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameState/SKGameState.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

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

	//틱활성화
	PrimaryActorTick.bCanEverTick = true;
}

void ASKPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	// PS->SetDAPlayerStat();
	SetPlayerStateTag();
	// SetWeapon(CurrentWeaponTag);
	//SetTraceSocket();

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

	if (bIsTracing)
	{
		PerformWeaponTrace(DeltaTime);
	}
}

void ASKPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(ASKPlayerCharacter, CurrentWeaponTag);
	DOREPLIFETIME(ASKPlayerCharacter, ComboState);
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


void ASKPlayerCharacter::OnLeftATKInput()
{
	if (!ComboState.bIsAttacking)
	{
		ResetComboIndex(1);
		ComboState.bIsAttacking = true;
		ComboState.bBufferedAttack = false;
		// ComboState.bCanNextCombo = true;
		ActivateLeftAttackGA();
		return;
	}

	// 2) 공격 중이지만 다음 공격 가능한 타이밍이면 → 콤보 진행 (GA 재발동)
	if (ComboState.bCanNextCombo)
	{
		ComboState.bBufferedAttack = false;
		IncreaseComboIndex();
		ActivateLeftAttackGA();
		return;
	}

	// 3) 공격 중인데 아직 콤보 타이밍이 아님 → 버퍼에 입력 저장
	ComboState.bBufferedAttack = true;
}

void ASKPlayerCharacter::ActivateLeftAttackGA()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC))
		return;

	FGameplayTag LeftAtkTag = GetLeftATKTag();
	FGameplayTagContainer Container;
	Container.AddTag(LeftAtkTag);

	ASC->TryActivateAbilitiesByTag(Container);
}

void ASKPlayerCharacter::StartAttackTrace()
{
	bIsTracing = true;
	ClearHitActor();
}

void ASKPlayerCharacter::StopAttackTrace()
{
	bIsTracing = false;
}

const TArray<AActor*> ASKPlayerCharacter::GetHitActors()
{
	return HitActors;
}

void ASKPlayerCharacter::ClearHitActor()
{
	HitActors.Empty();
}

void ASKPlayerCharacter::SetWeapon(FGameplayTag NewWeaponTag)
{
	//CurrentWeaponTag = NewWeaponTag;

	// if (!WeaponDataTable)
	// 	return;
	//
	// ResetLeftComboState();
	//
	//
	// const FSKWeaponDataRow* Row = WeaponDataTable->FindRow<FSKWeaponDataRow>(
	// 	CurrentWeaponTag.GetTagName(),
	// 	*CurrentWeaponTag.ToString()
	// );
	//
	//
	// if (Row)
	// {
	// 	CurrentWeaponTraceSockets = Row->TraceSockets;
	//
	// 	// 이전 소켓 위치 기억 배열도 맞춰 재설정 (중요!)
	// 	PreviousSocketLocations.SetNum(CurrentWeaponTraceSockets.Num());
	//
	// 	for (int32 i = 0; i < CurrentWeaponTraceSockets.Num(); i++)
	// 	{
	// 		PreviousSocketLocations[i] = GetMesh()->GetSocketLocation(CurrentWeaponTraceSockets[i]);
	// 	}
	// }
}

void ASKPlayerCharacter::SetTraceSocket()
{
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();

	CurrentWeaponTraceSockets =PS->GetTraceSocket();

	PreviousSocketLocations.SetNum(CurrentWeaponTraceSockets.Num());

	for (int32 i = 0; i < CurrentWeaponTraceSockets.Num(); i++)
	{
		PreviousSocketLocations[i] = GetMesh()->GetSocketLocation(CurrentWeaponTraceSockets[i]);
	}
}

void ASKPlayerCharacter::PerformWeaponTrace(float DeltaTime)
{
	if (CurrentWeaponTraceSockets.Num() == 0)
		return;

	// 이전 프레임 배열과 개수 매칭
	if (PreviousSocketLocations.Num() != CurrentWeaponTraceSockets.Num())
	{
		PreviousSocketLocations.SetNum(CurrentWeaponTraceSockets.Num());
		for (int32 i = 0; i < CurrentWeaponTraceSockets.Num(); i++)
		{
			PreviousSocketLocations[i] =
				GetMesh()->GetSocketLocation(CurrentWeaponTraceSockets[i]);
		}
	}


	for (int32 i = 0; i < CurrentWeaponTraceSockets.Num(); i++)
	{
		FVector PrevLocation = PreviousSocketLocations[i];
		FVector CurrLocation =
			GetMesh()->GetSocketLocation(CurrentWeaponTraceSockets[i]);

		DrawDebugLine(
			GetWorld(),
			PrevLocation,
			CurrLocation,
			FColor::Red,
			false,
			0.05f,
			0,
			2.0f
		);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit,
			PrevLocation,
			CurrLocation,
			ECC_Pawn,
			Params
		);

		if (bHit)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && !HitActors.Contains(HitActor))
			{
				HitActors.Add(HitActor); // 충돌한 액터만 저장
			}
		}

		PreviousSocketLocations[i] = CurrLocation;
	}
}

void ASKPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	SetTraceSocket();
}


void ASKPlayerCharacter::OnRep_ComboState()
{
}

int32 ASKPlayerCharacter::GetComboIndex()
{
	return ComboState.ComboIndex;
}

bool ASKPlayerCharacter::GetIsAttacking()
{
	return ComboState.bIsAttacking;
}

void ASKPlayerCharacter::ResetComboIndex(int32 ArgComboIndex)
{
	ComboState.ComboIndex = ArgComboIndex;
}


void ASKPlayerCharacter::IncreaseComboIndex(bool bLeft)
{
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();

	if (!IsValid(PS))
		return;

	int32 MaxCombo = PS->GetMaxComobo(bLeft);

	ComboState.ComboIndex++;

	// 최대 콤보 수 초과 방지
	if (ComboState.ComboIndex >= MaxCombo)
	{
		// ComboState.ComboIndex = 1;
		ResetComboIndex(1);
	}

	if (HasAuthority())
	{
		OnRep_ComboState(); // 이름에 맞게 수정 필요
	}
}

void ASKPlayerCharacter::OnATKEndNotify(bool bLeft)
{
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();

	if (!IsValid(PS))
		return;

	int32 MaxCombo = PS->GetMaxComobo(bLeft);


	// 1) 입력 버퍼가 있고, 아직 마지막 콤보가 아닐 때 → 콤보 이어가기
	if (ComboState.bBufferedAttack && ComboState.ComboIndex < MaxCombo)
	{
		ComboState.bBufferedAttack = false;
		IncreaseComboIndex();
		ActivateLeftAttackGA();
		return;
	}

	if (ComboState.ComboIndex >= MaxCombo)
	{
		ResetComboState();
		return;
	}

	// 3) 입력 버퍼 없음 → 콤보 종료
	ResetComboState();
}

bool ASKPlayerCharacter::CheckMaxComboIndex(bool bLeft)
{
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();

	if (!IsValid(PS))
		return false;

	int32 MaxCombo = PS->GetMaxComobo(bLeft);
	if (ComboState.ComboIndex >= MaxCombo)
		return true;
	return false;
}


FGameplayTag ASKPlayerCharacter::GetLeftATKTag() const
{
	FGameplayTag returnTag = FGameplayTag();

	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	
	if (!IsValid(PS))
	{
		return returnTag;
	}

	FGameplayTag PS_WeaponTag = PS->GetWeapontTag();
	
	if (PS_WeaponTag.MatchesTagExact(TAG_Weapon_Axe))
	{
		returnTag = TAG_Ability_LeftATK_Axe;
	}
	// else if (WeaponTag.MatchesTagExact(TAG_Weapon_Assassin))
	// {

	//     // return TAG_Ability_LeftATK_Assassin;
	// }


	return returnTag;
}

void ASKPlayerCharacter::ResetComboState()
{
	ResetComboIndex();
	ComboState.bIsAttacking = false;
	ComboState.bCanNextCombo = false;

	for (int32 i = 0; i < CurrentWeaponTraceSockets.Num(); i++)
	{
		PreviousSocketLocations[i] = GetMesh()->GetSocketLocation(CurrentWeaponTraceSockets[i]);
	}
}

void ASKPlayerCharacter::UpdateAnimInstanceComboState()
{
	ResetComboState();
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


void ASKPlayerCharacter::SetPlayerStateTag()
{
	// 주기적인 속도 체크를 위한 타이머 (틱 대신 사용)
	GetWorldTimerManager().SetTimer(MovementCheckTimer, this, &ASKPlayerCharacter::UpdateMovementTag, 0.2f, true);
	// 처음엔 Idle 상태 태그 추가
	SetLooseTag(AbilitySystemComponent, TAG_State_Movement_Idle, true);
}
