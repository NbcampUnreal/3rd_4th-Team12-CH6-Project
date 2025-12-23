// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/BattleComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Character/SKPlayerCharacter.h"
#include "GameAbilitySystem/Ability/SK_GA_LeftAttack.h"
#include "GameData/WeaponDataRow.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Weapon/SKWeaponData.h"

// Sets default values for this component's properties
UBattleComponent::UBattleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

}


// Called when the game starts
void UBattleComponent::BeginPlay()
{
	Super::BeginPlay();

	SetWeaponMesh_Init();
	
}

void UBattleComponent::Server_Input_Skill_01_Implementation()
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());

	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();
	
	FGameplayTagContainer Container;
	Container.AddTag(TAG_Ability_Skill_01);
	ASC->TryActivateAbilitiesByTag(Container);
}

void UBattleComponent::Server_Input_Skill_02_Implementation()
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());

	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();
	
	FGameplayTagContainer Container;
	Container.AddTag(TAG_Ability_Skill_02);
	ASC->TryActivateAbilitiesByTag(Container);
}

void UBattleComponent::Server_Input_Skill_03_Implementation()
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());

	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();
	
	FGameplayTagContainer Container;
	Container.AddTag(TAG_Ability_Skill_03);
	ASC->TryActivateAbilitiesByTag(Container);
}

void UBattleComponent::Client_SendSkillUIMessage_Implementation(int32 SkillNum, bool bSuccess)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	USKGameplayMessageSubsystem* MessageSubsystem =
		GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();

	if (!MessageSubsystem)
	{
		return;
	}

	FSkillUIMessage Message;
	Message.SkillNum = SkillNum;
	Message.bSuccess = bSuccess;

	MessageSubsystem->BroadcastMessage(
		TAG_Message_Channel_SkillUse,
		Message
	);
}

void UBattleComponent::NotifyUseSkill(int32 SkillNum, bool bSuccess)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	Client_SendSkillUIMessage(SkillNum, bSuccess);
}

UAnimMontage* UBattleComponent::GetLeftATKMontage(int32 Index)
{
	if (!CurrentWeaponData)
		return nullptr;

	if (CurrentWeaponData->LeftAttackMontages.IsValidIndex(Index))
		return CurrentWeaponData->LeftAttackMontages[Index];

	return nullptr;
}

UAnimMontage* UBattleComponent::GetSkillMontage(int32 Index)
{
	if (!CurrentWeaponData)
		return nullptr;

	if (CurrentWeaponData->SkillMontages.IsValidIndex(Index))
		return CurrentWeaponData->SkillMontages[Index];

	return nullptr;
}

UAnimMontage* UBattleComponent::GetRightATKMontage(int32 Index)
{
	if (!CurrentWeaponData)
		return nullptr;

	if (CurrentWeaponData->RightAttackMontages.IsValidIndex(Index))
		return CurrentWeaponData->RightAttackMontages[Index];

	return nullptr;
}


// Called every frame
void UBattleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsTracing)
	{
		PerformTrace(DeltaTime);
	}
}

void UBattleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBattleComponent, ComboState);
}

void UBattleComponent::StartTrace()
{
	SetIsTraced(true);
	ClearHitResult();

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

void UBattleComponent::StopTrace()
{
	SetIsTraced(false);
}

void UBattleComponent::PerformTrace(float DeltaTime)
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
		CapsuleRot, 
		FColor::Green,
		false,
		0.05f
	);


	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();

		if (HitActor && !HitActors.Contains(HitActor))
		{
			AddHitResult(Hit);
		}
	}

	PrevStart = CurrStart;
	PrevEnd = CurrEnd;
}


void UBattleComponent::ClearHitResult()
{
	HitResults.Reset();
}

void UBattleComponent::AddHitResult(const FHitResult& Hit)
{
	HitResults.Add(Hit);
}

void UBattleComponent::SetIsTraced(bool ArgIsTracing)
{
	bIsTracing = ArgIsTracing;
}

const TArray<FHitResult>& UBattleComponent::GetHitResult()
{
	return HitResults;
}

void UBattleComponent::Server_StartTrace_Implementation()
{
	StartTrace();
}

void UBattleComponent::Server_StopTrace_Implementation()
{
	StopTrace();
}

void UBattleComponent::Server_LeftATK_ApplyDamage_Implementation()
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();

	if (!ASC)
		return;

	// 현재 실행 중인 공격 GA 가져오기
	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.IsActive() &&
			Spec.Ability->GetClass()->IsChildOf(USK_GA_LeftAttack::StaticClass()))
		{
			USK_GA_LeftAttack* GA = Cast<USK_GA_LeftAttack>(Spec.GetPrimaryInstance());
			if (GA)
			{
				GA->OnStopAttackTrace_Server();
			}
			break;
		}
	}
}

void UBattleComponent::Server_ATKTYPE_ApplyDamage_Implementation(const FGameplayTag& AttackTag)
{
	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();

	if (!ASC)
		return;

	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.IsActive())
			continue;

		UGameplayAbility* Ability = Spec.GetPrimaryInstance();
		if (!Ability)
			continue;

		if (Ability->AbilityTags.HasTagExact(AttackTag))
		{
			USKGameplayAbility* SKGA = Cast<USKGameplayAbility>(Ability);
			if (SKGA)
			{
				SKGA->OnStopAttackTrace_Server();
			}
			return;
		}
	}
	
}

void UBattleComponent::SetWeaponMesh(USkeletalMeshComponent* InWeaponMesh)
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

void UBattleComponent::SetWeaponMesh_Init()
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

void UBattleComponent::InitializeWeaponSocket(const FSKWeaponDataRow* Row)
{
	if (!Row)
		return;

	ComboState.WeaponTag = FGameplayTag::RequestGameplayTag(Row->WeaponTag.GetTagName());

	TraceSockets = Row->TraceSockets;

}

void UBattleComponent::InitializeWeaponData(const FWeaponDataRow* Row)
{
	if (!Row)
		return;

	CurrentWeaponData = Row->WeaponData;

	UE_LOG(LogTemp, Error, TEXT("[DEBUG_00] CombatComponent %p | Owner %s"),
		   this,
		   *GetOwner()->GetName());

	UE_LOG(LogTemp, Error, TEXT("[DEBUG_00] CurrentWeaponData = %p"), CurrentWeaponData.Get());
}

FString UBattleComponent::FindWeaponTagName()
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

void UBattleComponent::OnRep_ComboState()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_ComboState: %s"),
	   *ComboState.WeaponTag.ToString());
}

