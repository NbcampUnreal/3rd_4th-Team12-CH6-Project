// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SKCombatComponent.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "GameAbilitySystem/Ability/SK_GA_LeftAttack_Axe.h"
#include "GameFramework/Character.h"
#include "GameData/WeaponDataRow.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

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

	// ...
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
	FGameplayTag returnTag = FGameplayTag();

	FGameplayTag ComboState_WeaponTag = ComboState.WeaponTag;

	if (ComboState_WeaponTag.MatchesTagExact(TAG_Weapon_Axe))
	{
		returnTag = TAG_Ability_LeftATK_Axe;
	}

	return returnTag;
}

int32 USKCombatComponent::GetComboIndex() const
{
	return ComboState.ComboIndex;
}

void USKCombatComponent::Server_LeftAttackInput_Implementation()
{
	// ASKPlayerState* PS = nullptr;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	// if (OwnerCharacter)
	// {
	// 	PS = OwnerCharacter->GetPlayerState<ASKPlayerState>();
	// }
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

void USKCombatComponent::Server_OnATKEndNotify_Implementation(bool bLeft)
{
	int32 MaxCombo = GetMaxComboIndex(bLeft);
	int32 RecComboIndex = GetComboIndex();

	if (ComboState.bBufferedAttack && RecComboIndex < MaxCombo)
	{
		ComboState.bBufferedAttack = false;
		ActivateLeftAttackGA(); // 이건 서버에서 실행됨
		return;
	}

	// 콤보 종료
	Server_ResetComboIndex(0);

	ComboState.bIsAttacking = false;
	ComboState.bCanNextCombo = false;


	FGameplayTagContainer CancelTags;
	CancelTags.AddTag(TAG_Ability_LeftATK); // 부모 태그

	ASKPlayerCharacter* SKPlayer = Cast<ASKPlayerCharacter>(GetOwner());
	UAbilitySystemComponent* ASC = SKPlayer->GetAbilitySystemComponent();
	ASC->CancelAbilities(&CancelTags, nullptr);
	// for (FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	// {
	// 	if (Spec.IsActive() &&
	// 		Spec.Ability->GetClass() == USK_GA_LeftAttack_Axe::StaticClass())
	// 	{
	// 		AbilitySystemComponent->CancelAbilityHandle(Spec.Handle);
	// 		break;
	// 	}
	// }
}

void USKCombatComponent::Client_PlayMontage_Implementation(UAnimMontage* Montage)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
		return;

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh)
		return;

	UAnimInstance* AIM = Mesh->GetAnimInstance();
	if (AIM)
	{
		AIM->Montage_Play(Montage, 1.0f);
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
	if (TraceSockets.Num() == 0)
		return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
		return;

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh)
		return;

	// 이전 프레임 배열과 개수 매칭
	if (PrevSocketLocations.Num() != TraceSockets.Num())
	{
		PrevSocketLocations.SetNum(TraceSockets.Num());
		for (int32 i = 0; i < TraceSockets.Num(); i++)
		{
			PrevSocketLocations[i] =
				Mesh->GetSocketLocation(TraceSockets[i]);
		}
	}


	for (int32 i = 0; i < TraceSockets.Num(); i++)
	{
		FVector PrevLocation = PrevSocketLocations[i];
		FVector CurrLocation =
			Mesh->GetSocketLocation(TraceSockets[i]);

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
		Params.AddIgnoredActor(OwnerCharacter);

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

		PrevSocketLocations[i] = CurrLocation;
	}
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

	ComboState.WeaponTag = Row->WeaponTag;
	
	TraceSockets = Row->TraceSockets;
	MaxLeftComboIndex = Row->MaxLeftCombo;
	MaxRightComboIndex = Row->MaxRightCombo;
}
