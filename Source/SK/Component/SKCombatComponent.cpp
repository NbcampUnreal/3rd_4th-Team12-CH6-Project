// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SKCombatComponent.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Controller/SKPlayerController.h"
#include "GameAbilitySystem/Ability/SK_GA_LeftAttack_Axe.h"
#include "GameFramework/Character.h"
#include "GameData/WeaponDataRow.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/SKWeaponData.h"
#include "Utility/SKNativeGameplayTags.h"


// Sets default values for this component's properties
USKCombatComponent::USKCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void USKCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetWeaponMesh_Init();
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
	else if (WeaponTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Weapon.Assassin")))
	{
		TargetName = "Assassin";
	}
	else if (WeaponTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Weapon.GreatSword")))
	{
		TargetName = "GreatSword";
	}
	else if (WeaponTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Weapon.Dual")))
	{
		TargetName = "Dual";
	}
	else if (WeaponTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Weapon.Dual")))
	{
		TargetName = "Dual";
	}
	else if (WeaponTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Weapon.Katana")))
	{
		TargetName = "Katana";
	}
	else if (WeaponTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Weapon.Spear")))
	{
		TargetName = "Spear";
	}


	return TargetName;
}

void USKCombatComponent::OnRep_ComboState()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_ComboState: %s"),
	       *ComboState.WeaponTag.ToString());
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
	FGameplayTag returnTag = TAG_Ability_LeftATK;

	return returnTag;
}

FGameplayTag USKCombatComponent::GetATKMeleeTag(bool bLeft) const
{
	FGameplayTag returnTag = bLeft ? TAG_State_Action_ATK_LeftMelee : TAG_State_Action_ATK_RightMelee;

	return returnTag;
}

FGameplayTag USKCombatComponent::GetWeaponTag() const
{
	return ComboState.WeaponTag;
}

int32 USKCombatComponent::GetComboIndex() const
{
	return ComboState.ComboIndex;
}

void USKCombatComponent::StopMontage_Local(float InBlendOut)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	UAnimInstance* AIM = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AIM)
	{
		AIM->Montage_Stop(InBlendOut);
	}
}

void USKCombatComponent::Server_Input_Right_Implementation()
{
}

void USKCombatComponent::Server_Input_Skill_01_Implementation()
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());

	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();
	
	FGameplayTagContainer Container;
	Container.AddTag(TAG_Ability_Skill_01);
	ASC->TryActivateAbilitiesByTag(Container);
}

void USKCombatComponent::Multicast_StopMontage_Implementation(float InBlendOut)
{
	StopMontage_Local(InBlendOut);
}

void USKCombatComponent::Client_StopMontage_Implementation(float InBlendOut)
{
	StopMontage_Local(InBlendOut);
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

	// else if (ComboState.bCanNextCombo)
	// {
	// 	ComboState.bBufferedAttack = false;
	// }

	else
	{
		ComboState.bBufferedAttack = true;
		return;
	}

	FGameplayTagContainer Container;
	Container.AddTag(GetLeftATKTag());

	ASC->TryActivateAbilitiesByTag(Container);
	// bool success = ASC->TryActivateAbilitiesByTag(Container);

	// // Current owned tags
	// FGameplayTagContainer OwnedTags;
	// ASC->GetOwnedGameplayTags(OwnedTags);
	// UE_LOG(LogTemp, Error, TEXT("[LEFTATKINPUT]:OwnedTags: %s"), *OwnedTags.ToStringSimple());


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


void USKCombatComponent::Server_OnATKEndNotify_Implementation(bool bLeft)
{
	int32 MaxCombo = GetMaxComboIndex(bLeft);
	int32 RecComboIndex = GetComboIndex();

	if (ComboState.bBufferedAttack && RecComboIndex < MaxCombo)
	{
		ComboState.bBufferedAttack = false;

		Multicast_ActivateLeftGA();
		//ActivateLeftAttackGA();
		return;
	}

	// 콤보 종료
	Server_ResetComboIndex(0);

	ComboState.bIsAttacking = false;
	ComboState.bCanNextCombo = false;

	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	if (!SKPlayer) return;

	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();
	if (!ASC) return;

	// ASC->RemoveLooseGameplayTag(GetATKMeleeTag());
	// Multicast_RemoveATKTag();

	UE_LOG(LogTemp, Error, TEXT("[ATK_END] Buffered=%d | Combo=%d / %d"),
	       ComboState.bBufferedAttack,
	       RecComboIndex,
	       MaxCombo);
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
	// OnRep_ComboState(); 
	UE_LOG(LogTemp, Error, TEXT("[SERVER] Increase -> %d"), ComboState.ComboIndex);
}

void USKCombatComponent::Server_ResetComboIndex_Implementation(int32 NewIndex)
{
	UE_LOG(LogTemp, Error, TEXT("[SERVER] Reset -> %d"), NewIndex);
	ComboState.ComboIndex = NewIndex;
}


void USKCombatComponent::Server_SetWeaponTag_Implementation(FGameplayTag NewWeaponTag)
{
	ComboState.WeaponTag = NewWeaponTag;
	OnRep_ComboState();
}


void USKCombatComponent::Server_TryActivateGA_Implementation(const FGameplayTag& Tag)
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	if (!SKPlayer) return;

	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayTagContainer Container;
	Container.AddTag(Tag);

	// 서버에서 GA 실행
	ASC->TryActivateAbilitiesByTag(Container);
}

void USKCombatComponent::Multicast_PlayLeftAttackMontage_Implementation(UAnimMontage* Montage, FName SectionName)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	UAnimInstance* Anim = OwnerChar->GetMesh()->GetAnimInstance();
	if (!Anim) return;

	Anim->Montage_Play(Montage);
	Anim->Montage_JumpToSection(SectionName, Montage);
}

void USKCombatComponent::Server_StartTrace_Implementation()
{
	StartTrace();
}

void USKCombatComponent::Server_StopTrace_Implementation()
{
	StopTrace();
}

void USKCombatComponent::Multicast_ActivateLeftGA_Implementation()
{
	ActivateLeftAttackGA();
}

void USKCombatComponent::Multicast_PlayMontage_Implementation(UAnimMontage* Montage, FName SectionName)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	UAnimInstance* Anim = OwnerChar->GetMesh()->GetAnimInstance();
	if (!Anim) return;

	Anim->Montage_Play(Montage);
	Anim->Montage_JumpToSection(SectionName, Montage);
}

void USKCombatComponent::SetWeaponTag(const FGameplayTag& NewTag)
{
	if (GetOwnerRole() == ROLE_Authority) // 서버에서만 세팅
	{
		ComboState.WeaponTag = NewTag;
	}
	else
	{
		// 클라이언트가 호출하면 서버 RPC로 전달
		Server_SetWeaponTag(NewTag);
	}
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
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
		return;
	if (!IsValid(WeaponMesh))
	{
		return;
	}

	FVector CurrStart = WeaponMesh->GetSocketLocation(WeaponStartSocket);
	FVector CurrEnd = WeaponMesh->GetSocketLocation(WeaponEndSocket);

	// 캡슐 반지름 & 하프헛(길이/2)
	float Radius = CapsuleRadius;
	float HalfHeight = CapsultHalfHeight;

	// Start→End 방향 벡터
	FVector TraceDir = CurrEnd - CurrStart;

	// 캡슐 회전 (Start→End 방향으로 캡슐 축을 회전시킴)
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceDir).ToQuat();
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

void USKCombatComponent::InitializeWeaponSocket(const FSKWeaponDataRow* Row)
{
	if (!Row)
		return;

	ComboState.WeaponTag = FGameplayTag::RequestGameplayTag(Row->WeaponTag.GetTagName());

	TraceSockets = Row->TraceSockets;
	MaxLeftComboIndex = Row->MaxLeftCombo;
	MaxRightComboIndex = Row->MaxRightCombo;
}

void USKCombatComponent::InitializeWeaponData(const FWeaponDataRow* Row)
{
	if (!Row)
		return;

	CurrentWeaponData = Row->WeaponData;

	UE_LOG(LogTemp, Error, TEXT("[DEBUG_00] CombatComponent %p | Owner %s"),
	       this,
	       *GetOwner()->GetName());

	UE_LOG(LogTemp, Error, TEXT("[DEBUG_00] CurrentWeaponData = %p"), CurrentWeaponData.Get());
}

UAnimMontage* USKCombatComponent::GetLeftAttackMontage(int32 Index)
{
	if (!CurrentWeaponData)
		return nullptr;

	if (CurrentWeaponData->LeftAttackMontages.IsValidIndex(Index))
		return CurrentWeaponData->LeftAttackMontages[Index];

	return nullptr;
}

UAnimMontage* USKCombatComponent::GetSkillMontage(int32 Index)
{
	if (!CurrentWeaponData)
		return nullptr;

	if (CurrentWeaponData->SkillMontages.IsValidIndex(Index))
		return CurrentWeaponData->SkillMontages[Index];

	return nullptr;
}


void USKCombatComponent::SetWeaponMesh(USkeletalMeshComponent* InWeaponMesh)
{
	WeaponMesh = InWeaponMesh;

	if (!WeaponMesh)
	{
		// 무기 해제 처리
		PrevStart = FVector::ZeroVector;
		PrevEnd = FVector::ZeroVector;
		return;
	}

	PrevStart = WeaponMesh->GetSocketLocation(WeaponStartSocket);
	PrevEnd = WeaponMesh->GetSocketLocation(WeaponEndSocket);
	
}

void USKCombatComponent::SetWeaponMesh_Init()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

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

void USKCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USKCombatComponent, ComboState);
}

void USKCombatComponent::Multicast_RemoveATKTag_Implementation(bool bLeft)
{
	FGameplayTag returnTag = GetATKMeleeTag(bLeft);

	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();

	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(returnTag);
	}
}
