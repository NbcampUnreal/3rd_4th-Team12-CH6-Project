// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SKCombatComponent.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Controller/SKPlayerController.h"
#include "GameAbilitySystem/Ability/SK_GA_LeftAttack_Axe.h"
#include "GameFramework/Character.h"
#include "GameData/WeaponDataRow.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Weapon/SKWeaponData.h"

// Sets default values for this component's properties
USKCombatComponent::USKCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USKCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	// 모든 스태틱메쉬 컴포넌트 가져오기
	TArray<USkeletalMeshComponent*> MeshComponents;
	OwnerCharacter->GetComponents<USkeletalMeshComponent>(MeshComponents);

	FName TargetTag = FName(*FindWeaponTagName()); // FString → FName 변환

	for (USkeletalMeshComponent* Comp : MeshComponents)
	{
		if (Comp && Comp->ComponentHasTag(TargetTag))
		{
			SetWeaponMesh(Comp);
			break;
		}
	}
}

void USKCombatComponent::ActivateLeftAttackGA()
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();

	if (!IsValid(ASC))
		return;

	FGameplayTag LeftAtkTag = GetLeftATKTag();
	FGameplayTagContainer Container;
	Container.AddTag(LeftAtkTag);


	UE_LOG(LogTemp, Error, TEXT("[ActivateLeftAttackGA] "));
	ASC->TryActivateAbilitiesByTag(Container);
}

int32 USKCombatComponent::GetMaxComboIndex(bool bLeft)
{
	int32 result = 0;
	if (bLeft)
		result = MaxLeftComboIndex;
	else
		result = MaxRightComboIndex;
	return result;
}

FString USKCombatComponent::FindWeaponTagName()
{
	const FGameplayTag& WeaponTag = ComboState.WeaponTag;

	FString TargetName;

	if (WeaponTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Weapon.TwoHanded")))
	{
		TargetName = "TwoHanded";
	}
	else if (WeaponTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Weapon.Warrior")))
	{
		TargetName = "Warrior";
	}

	return TargetName;
}

FRotator USKCombatComponent::GetDodgeRotator()
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return FRotator();
	
	ASKPlayerController* PC = Cast<ASKPlayerController>(Char->GetController());
	if (!PC) return FRotator();

	const FRotator ControllerRot = PC->GetControlRotation();
	const FVector2D InputVector = PC->CurrentInputVector;

	FRotator TempRot = ControllerRot;
	TempRot.Roll = 0.f;
	TempRot.Pitch = 0.f;

	const FVector TempForward = FRotationMatrix(TempRot).GetUnitAxis(EAxis::X);
	const FVector TempRight = FRotationMatrix(TempRot).GetUnitAxis(EAxis::Y);

	const FVector TargetVector = TempForward * InputVector.X + TempRight * InputVector.Y;
	const FRotator TargetRot = TargetVector.GetSafeNormal().Rotation();
	
	return TargetRot;
}


// Called every frame
void USKCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsTracing)
	{
		PerformTrace(DeltaTime);
	}
}

bool USKCombatComponent::GetIsAttacking()
{
	return ComboState.bIsAttacking;
}

void USKCombatComponent::ResetComboState()
{
	ComboState.bIsAttacking = false;
	ComboState.bBufferedAttack = false;
	ComboState.bCanNextCombo = false;
}

bool USKCombatComponent::CheckMaxComboIndex(bool bLeft)
{
	bool result = false;
	int32 MaxComboIndex = 0;
	if (bLeft)
	{
		MaxComboIndex = MaxLeftComboIndex;
	}
	else
	{
		MaxComboIndex = MaxRightComboIndex;
	}

	if (ComboState.ComboIndex >= MaxComboIndex - 1)
		result = true;

	return result;
}

FGameplayTag USKCombatComponent::GetLeftATKTag() const
{
	// FGameplayTag returnTag = FGameplayTag();

	FGameplayTag returnTag = TAG_Ability_LeftATK;
	// FGameplayTag ComboState_WeaponTag = ComboState.WeaponTag;
	//
	// if (ComboState_WeaponTag.MatchesTagExact(TAG_Weapon_TwoHanded))
	// {
	// 	returnTag = TAG_Ability_LeftATK_TwoHanded;
	// }

	return returnTag;
}

int32 USKCombatComponent::GetComboIndex() const
{
	return ComboState.ComboIndex;
}

void USKCombatComponent::Server_LeftAttackInput_Implementation()
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();

	if (!ComboState.bIsAttacking)
	{
		ComboState.bIsAttacking = true;
		ComboState.bBufferedAttack = false;
	}

	else if (ComboState.bCanNextCombo)
	{
		ComboState.bBufferedAttack = false;
	}

	else
	{
		ComboState.bBufferedAttack = true;
		return;
	}
	FGameplayTagContainer Container;
	Container.AddTag(GetLeftATKTag());

	ASC->TryActivateAbilitiesByTag(Container);
}

void USKCombatComponent::Server_Notify_StopAttackTrace_Implementation()
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();

	if (!ASC)
		return;

	// 현재 실행 중인 공격 GA 가져오기
	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.IsActive() &&
			Spec.Ability->GetClass()->IsChildOf(USK_GA_LeftAttack_Axe::StaticClass()))
		{
			USK_GA_LeftAttack_Axe* GA = Cast<USK_GA_LeftAttack_Axe>(Spec.GetPrimaryInstance());
			if (GA)
			{
				GA->OnStopAttackTrace_Server();
			}
			break;
		}
	}
}

static FString TagsToString(const FGameplayTagContainer& Tags)
{
	FString Result;

	for (const FGameplayTag& Tag : Tags)
	{
		Result += Tag.ToString();
		Result += TEXT(" | ");
	}

	if (Result.Len() == 0)
	{
		Result = TEXT("(EMPTY)");
	}

	return Result;
}


void USKCombatComponent::Server_OnATKEndNotify_Implementation(bool bLeft)
{
	int32 MaxCombo = GetMaxComboIndex(bLeft);
	int32 RecComboIndex = GetComboIndex();

	if (ComboState.bBufferedAttack && RecComboIndex < MaxCombo)
	{
		ComboState.bBufferedAttack = false;
		ActivateLeftAttackGA(); 
		return;
	}

	// 콤보 종료
	Server_ResetComboIndex(0);

	ComboState.bIsAttacking = false;
	ComboState.bCanNextCombo = false;


	FGameplayTagContainer CancelTags;
	CancelTags.AddTag(TAG_Ability_LeftATK_Cancel); // 부모 태그


	
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();


	
	ASC->CancelAbilities(&CancelTags, nullptr);

	UAnimInstance* AnimInstance = SKPlayer->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.15f);  // BlendOut 0.15f 정도 추천

	}
}

void USKCombatComponent::Client_PlayMontage_Implementation(UAnimMontage* Montage, FName StartSection)
{
	if (!Montage)
		return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
		return;

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh)
		return;
	UAnimInstance* AIM = Mesh->GetAnimInstance();
	if (AIM)
	{
		AIM->Montage_Play(Montage, 1.f);
		if (StartSection != NAME_None)
		{
			AIM->Montage_JumpToSection(StartSection, Montage);
		}
	}
}

void USKCombatComponent::Server_IncreaseComboIndex_Implementation(bool bLeft)
{
	ComboState.ComboIndex++;
	UE_LOG(LogTemp, Error, TEXT("[SERVER] Increase -> %d"), ComboState.ComboIndex);
}

void USKCombatComponent::Server_ResetComboIndex_Implementation(int32 NewIndex)
{
	UE_LOG(LogTemp, Error, TEXT("[SERVER] Reset -> %d"), NewIndex);
	ComboState.ComboIndex = NewIndex;
}


void USKCombatComponent::StartTrace()
{
	SetIsTraced(true);
	ClearHitActor();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
		return;

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh)
		return;

	PrevSocketLocations.SetNum(TraceSockets.Num());

	for (int32 i = 0; i < TraceSockets.Num(); i++)
	{
		PrevSocketLocations[i] = Mesh->GetSocketLocation(TraceSockets[i]);
	}
}

void USKCombatComponent::StopTrace()
{
	SetIsTraced(false);
}

void USKCombatComponent::PerformTrace(float DeltaTime)
{
	// if (TraceSockets.Num() == 0)
	// 	return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
		return;

	FVector CurrStart = WeaponMesh->GetSocketLocation(WeaponStartSocket);
	FVector CurrEnd = WeaponMesh->GetSocketLocation(WeaponEndSocket);

	// 캡슐 반지름 & 하프헛(길이/2)
	float Radius = CapsuleRadius;
	float HalfHeight = CapsultHalfHeight;

	// Start→End 방향 벡터
	FVector TraceDir = CurrEnd - CurrStart;

	// 캡슐 회전 (Start→End 방향으로 캡슐 축을 회전시킴)
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceDir).ToQuat();

	// Sweep는 중심과 회전을 기준으로 하기 때문에
	// 캡슐 중심 좌표 구하기 (Start와 End 중간지점)
	FVector CapsuleCenter = (CurrStart + CurrEnd) * 0.5f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());


	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		PrevStart,
		CurrStart,
		CapsuleRot, 
		ECC_Pawn,
		FCollisionShape::MakeCapsule(Radius, HalfHeight),
		Params
	);


	DrawDebugCapsule(
			GetWorld(),
			CapsuleCenter,
			HalfHeight,
			Radius,
			CapsuleRot, // ★ 핵심: 회전 적용
			FColor::Green,
			false,
			0.05f
		);

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
		}
	}

	PrevStart = CurrStart;
	PrevEnd = CurrEnd;
}


void USKCombatComponent::ResetCombo(bool bLeft)
{
}

void USKCombatComponent::ClearHitActor()
{
	HitActors.Empty();
}

void USKCombatComponent::SetIsTraced(bool ArgIsTracing)
{
	bIsTracing = ArgIsTracing;
}

const TArray<AActor*>& USKCombatComponent::GetHitActors()
{
	return HitActors;
}

void USKCombatComponent::InitializeWeaponData(const FSKWeaponDataRow* Row)
{
	if (!Row)
		return;

	ComboState.WeaponTag = FGameplayTag::RequestGameplayTag(Row->WeaponTag.GetTagName());

	TraceSockets = Row->TraceSockets;
	MaxLeftComboIndex = Row->MaxLeftCombo;
	MaxRightComboIndex = Row->MaxRightCombo;
}

const FWeaponDataRow* USKCombatComponent::GetWeaponData() const
{
	if (!WeaponDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDataTable is null in CombatComponent!"));
		return nullptr;
	}

	const FGameplayTag& WeaponTag = ComboState.WeaponTag;

	FString FullName = WeaponTag.GetTagName().ToString();
	FString LastName;
	FullName.Split(TEXT("."), nullptr, &LastName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	FName RowName(*LastName);

	const FWeaponDataRow* Row =
		WeaponDataTable->FindRow<FWeaponDataRow>(RowName, TEXT(""));

	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDataRow NOT FOUND: %s (RowName = %s)"),
		       *WeaponTag.ToString(),
		       *RowName.ToString());
		return nullptr;
	}

	return Row;
}

void USKCombatComponent::SetWeaponMesh(USkeletalMeshComponent* InWeaponMesh)
{
	if (!InWeaponMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("SetWeaponMesh: InWeaponMesh is NULL"));
		return;
	}

	WeaponMesh = InWeaponMesh;

	// 초기 Prev 값 설정
	PrevStart = WeaponMesh->GetSocketLocation(WeaponStartSocket);
	PrevEnd = WeaponMesh->GetSocketLocation(WeaponEndSocket);
}
