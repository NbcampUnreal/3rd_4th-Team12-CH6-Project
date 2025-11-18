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

	SetDAPlayerStat();
	SetPlayerStateTag();
	SetWeapon(CurrentWeaponTag);

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

void ASKPlayerCharacter::SetSprinting(bool bSprinting)
{
	bIsSprinting = bSprinting;
	float WalkSpeed = AttributeSet->GetSpeed();
	float SprintSpeed = AttributeSet->GetSprintWeight() * WalkSpeed;
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

void ASKPlayerCharacter::OnLeftATKInput()
{
	FGameplayTag WeaponTag = CurrentWeaponTag;

	bool& bIsAttacking = IsAttackingMap.FindOrAdd(WeaponTag);
	bool& bCanNextCombo = CanNextComboMap.FindOrAdd(WeaponTag);
	int32& ComboIndex = LeftComboIndexMap.FindOrAdd(WeaponTag);

	if (!bIsAttacking)
	{
		ComboIndex = 1;
		bIsAttacking = true;
		bBufferedAttack = false;

		ActivateLeftAttackGA();
		return;
	}

	// 2) 공격 중이지만 다음 공격 가능한 타이밍이면 → 콤보 진행 (GA 재발동)
	if (bCanNextCombo)
	{
		bBufferedAttack = false;
		ComboIndex++;
		ActivateLeftAttackGA();
		return;
	}

	// 3) 공격 중인데 아직 콤보 타이밍이 아님 → 버퍼에 입력 저장
	bBufferedAttack = true;
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
	CurrentWeaponTag = NewWeaponTag;

	if (!WeaponDataTable)
		return;

	ResetLeftComboState();


	const FSKWeaponDataRow* Row = WeaponDataTable->FindRow<FSKWeaponDataRow>(
		CurrentWeaponTag.GetTagName(),
		*CurrentWeaponTag.ToString()
	);


	if (Row)
	{
		CurrentWeaponTraceSockets = Row->TraceSockets;

		// 이전 소켓 위치 기억 배열도 맞춰 재설정 (중요!)
		PreviousSocketLocations.SetNum(CurrentWeaponTraceSockets.Num());

		for (int32 i = 0; i < CurrentWeaponTraceSockets.Num(); i++)
		{
			PreviousSocketLocations[i] = GetMesh()->GetSocketLocation(CurrentWeaponTraceSockets[i]);
		}
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

void ASKPlayerCharacter::OnLeftAttackEndNotify()
{
	const int RecentCombo = LeftComboIndexMap.FindOrAdd(CurrentWeaponTag);
	const int MaxCombo = LeftMaxComboMap.FindOrAdd(CurrentWeaponTag);

	// 1) 입력 버퍼가 있고, 아직 마지막 콤보가 아닐 때 → 콤보 이어가기
	if (bBufferedAttack && RecentCombo < MaxCombo)
	{
		bBufferedAttack = false;
		IncreseLeftComboIndex();
		ActivateLeftAttackGA();
		return;
	}

	// 2) 마지막 콤보 도달 → 무조건 종료
	if (RecentCombo >= MaxCombo)
	{
		ResetLeftComboState();
		return;
	}

	// 3) 입력 버퍼 없음 → 콤보 종료
	ResetLeftComboState();
}

void ASKPlayerCharacter::ResetLeftComboState(FGameplayTag WeaponTag)
{
	if (!WeaponTag.IsValid())
	{
		WeaponTag = CurrentWeaponTag;
	}

	LeftComboIndexMap.FindOrAdd(WeaponTag) = 0;
	IsAttackingMap.FindOrAdd(WeaponTag) = false;
	CanNextComboMap.FindOrAdd(WeaponTag) = false;

	if (CurrentWeaponTag.MatchesTagExact(TAG_Weapon_Axe))
	{
		LeftMaxComboMap.FindOrAdd(WeaponTag, SKConstant::LeftMaxCombo_Axe);
	}

	for (int32 i = 0; i < CurrentWeaponTraceSockets.Num(); i++)
	{
		PreviousSocketLocations[i] = GetMesh()->GetSocketLocation(CurrentWeaponTraceSockets[i]);
	}
}

bool ASKPlayerCharacter::GetIsLeftAttackingByTag() const
{
	const bool* bIsAttackingPtr = IsAttackingMap.Find(CurrentWeaponTag);


	if (bIsAttackingPtr)
	{
		return *bIsAttackingPtr;
	}

	return false;
}

int ASKPlayerCharacter::GetIsLeftComboIndexByTag() const
{
	if (!CurrentWeaponTag.IsValid())
		return 0;

	const int* ComboIndexPtr = LeftComboIndexMap.Find(CurrentWeaponTag);

	if (ComboIndexPtr)
	{
		return *ComboIndexPtr;
	}

	return 0;
}

void ASKPlayerCharacter::IncreseLeftComboIndex()
{
	int& ComboIndex = LeftComboIndexMap.FindOrAdd(CurrentWeaponTag);
	const int32* MaxComboPtr = LeftMaxComboMap.Find(CurrentWeaponTag);
	
	int32 MaxCombo = MaxComboPtr ? *MaxComboPtr -1 : 1;

	// 콤보 증가
	ComboIndex++;

	// 최대 콤보 수 초과 방지
	if (ComboIndex > MaxCombo)
	{
		UE_LOG(LogTemp, Warning,
			   TEXT("ComboIndex exceeded MaxCombo! Clamping.  Index=%d  Max=%d"),
			   ComboIndex, MaxCombo);
		ComboIndex = 1;
	}
}

bool ASKPlayerCharacter::CheckMaxLeftComboIndex()
{
	const int* RecentComboIndex = LeftComboIndexMap.Find(CurrentWeaponTag);
	const int* MaxComboIndex = LeftMaxComboMap.Find(CurrentWeaponTag);
	if (RecentComboIndex && MaxComboIndex)
	{
		return (*RecentComboIndex == *MaxComboIndex);
	}
	return false;
}

FGameplayTag ASKPlayerCharacter::GetLeftATKTag() const
{
	FGameplayTag returnTag = FGameplayTag();
	if (!CurrentWeaponTag.IsValid())
	{
		return returnTag;
	}

	if (CurrentWeaponTag.MatchesTagExact(TAG_Weapon_Axe))
	{
		returnTag = TAG_Ability_LeftATK_Axe;
	}
	// else if (WeaponTag.MatchesTagExact(TAG_Weapon_Assassin))
	// {

	//     // return TAG_Ability_LeftATK_Assassin;
	// }


	return returnTag;
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
