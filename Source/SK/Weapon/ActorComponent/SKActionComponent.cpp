#include "SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Controller/SKPlayerController.h"
#include "Animation/SKPlayerAnimInstance.h"
#include "Component/BattleComponent.h"
#include "Interaction/Ability/SK_GA_Unequip.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/SKPlayerState.h"
#include "Weapon/SKWeaponData.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Components/CapsuleComponent.h"
#include "GameData/SKGameConstant.h"

USKActionComponent::USKActionComponent()
	: CurrentWeaponAnimData(nullptr)
{
	SetIsReplicatedByDefault(true);
}

void USKActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USKActionComponent, CurrentWeaponAnimData);
	DOREPLIFETIME(USKActionComponent, CurrentInputVector);
	DOREPLIFETIME(USKActionComponent, CurrentMoveDirection);
	DOREPLIFETIME(USKActionComponent, WeaponActors);
	DOREPLIFETIME(USKActionComponent, bIgnoreCollision);
}

void USKActionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void USKActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USKActionComponent::Multicast_SetWeaponData_Implementation(USKWeaponData* NewWeaponData)
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;
	UBattleComponent* BattleComponent = Char->GetBattleComponent();
	if (!BattleComponent) return;
	BattleComponent->CurrentWeaponData = NewWeaponData;
}

void USKActionComponent::CheckAutoUnEquipped()
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char || !Char->HasAuthority()) return;
	
	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (!ASC) return;

	UAnimInstance* AnimInstance = Char->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (AnimInstance->IsAnyMontagePlaying())
	{
		return;
	}

	const FGameplayTag EquipTag =
		FGameplayTag::RequestGameplayTag(TEXT("State.Condition.Equip"));

	float Now = GetWorld()->GetTimeSeconds();

	if (ASC->HasMatchingGameplayTag(EquipTag))
	{
		if (Now - LastCombatTime > AutoUnequipDelay)
		if (Now - LastCombatTime > AutoUnequipDelay)
		{
			ASC->TryActivateAbilityByClass(USK_GA_Unequip::StaticClass());
		}
	}
}

void USKActionComponent::Server_SetIgnoreCollision_Implementation(bool bIgnore)
{
	bIgnoreCollision = bIgnore;
	ApplyCollisionSetting();
}

void USKActionComponent::OnRep_IgnoreCollision()
{
	ApplyCollisionSetting();
}

void USKActionComponent::ApplyCollisionSetting()
{
	ACharacter* Char = Cast<ACharacter>(GetOwner());
	if (!Char) return;
	
	UCapsuleComponent* Capsule = Char->GetCapsuleComponent();
	
	Capsule->SetCollisionResponseToChannel(
		SKConstant::ECC_Interactable,
		bIgnoreCollision ? ECR_Ignore : ECR_Block
	);
}

void USKActionComponent::OnOwnerPossessed()
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char || !Char->HasAuthority()) return;

	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (!ASC) return;	

	ASKPlayerState* PlayerState = Cast<ASKPlayerState>(Char->GetPlayerState());
	if (IsValid(PlayerState))
	{
		if (PlayerState->bIsFirstSpawned)
		{
			PlayerState->bIsFirstSpawned = false;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("??????"))
			// 무기 장착시 태그 해제
			const FWeaponDataRow* WeaponDataRow = PlayerState->GetWeaponDataRow();
			if (!WeaponDataRow)
			{
				UE_LOG(LogTemp, Warning, TEXT("WeaponData Is Null"))
			}
			UE_LOG(LogTemp, Warning, TEXT("WeaponData, %s"), *WeaponDataRow->WeaponAnimData.GetName())
	
			Multicast_SetWeaponAnimData(WeaponDataRow->WeaponAnimData);
			Multicast_SetWeaponData(WeaponDataRow->WeaponData);
			if (ASC)
			{
				USKWeaponAnimData* AnimData = WeaponDataRow->WeaponAnimData;
				if (!AnimData) return;
		
				if (AnimData->UnequipGE && AnimData->EquipGE)
				{
					ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->UnequipGE, ASC, 1);
					ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->EquipGE, ASC, 1);
			
					FGameplayEffectSpecHandle UnequipGESpecHandle = ASC->MakeOutgoingSpec(AnimData->UnequipGE, 1.f, ASC->MakeEffectContext());
					if (UnequipGESpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*UnequipGESpecHandle.Data.Get());
					}
				}
			}
			return;
		}
	}

	GetWorld()->GetTimerManager().SetTimer(AutoUnEquippedTimerHandle, this, &USKActionComponent::CheckAutoUnEquipped, 0.33f, true);
	// 초기 태그 설정
	FGameplayTag UnarmedTag = FGameplayTag::RequestGameplayTag(TEXT("Weapon.Unarmed"));
	ASC->AddLooseGameplayTag(UnarmedTag);
	
	if (IsValid(PlayerState))
	{
		PlayerState->SetCurWeaponTag(UnarmedTag);
	}
	const FWeaponDataRow* WeaponDataRow = PlayerState->GetWeaponDataRow();
	if (!WeaponDataRow) return;
	UE_LOG(LogTemp, Warning, TEXT("WeaponData, %s"), *WeaponDataRow->WeaponAnimData.GetName())
	
	CurrentWeaponAnimData = WeaponDataRow->WeaponAnimData;
	
	Char->GetMesh()->SetAnimInstanceClass(CurrentWeaponAnimData->AnimInstance);
}

void USKActionComponent::AttachWeapon(const TArray<FName> SocketNames)
{
	if (WeaponActors.IsEmpty() || SocketNames.IsEmpty()) return;

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;

	USkeletalMeshComponent* Mesh = Char->GetMesh();
	if (!Mesh) return;

	const int32 Count = FMath::Min(WeaponActors.Num(), SocketNames.Num());
	
	USceneComponent* AttachTarget = GetOwner()->GetRootComponent();
	for (int i = 0; i < Count; i++)
	{
		WeaponActors[i]->AttachToComponent(
			Mesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketNames[i]);
	}
}

void USKActionComponent::OnCombatStart()
{
	UE_LOG(LogTemp, Display, TEXT("OnCombatAction"));

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;
	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (!ASC) return;

	const FGameplayTag EquipTag = FGameplayTag::RequestGameplayTag(TEXT("State.Condition.Equip"));

	// ASC->RemoveLooseGameplayTag(EquipTag);
	// ASC->AddLooseGameplayTag(EquipTag);

	USKWeaponAnimData* AnimData = GetWeaponAnimData();
	if (!AnimData) return;
		
	if (AnimData->UnequipGE && AnimData->EquipGE)
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->UnequipGE, ASC, 1);
		ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->EquipGE, ASC, 1);
				
		FGameplayEffectSpecHandle UnequipGESpecHandle = ASC->MakeOutgoingSpec(AnimData->EquipGE, 1.f, ASC->MakeEffectContext());
		if (UnequipGESpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*UnequipGESpecHandle.Data.Get());
		}
	}
			
	AttachWeapon(GetWeaponAnimData()->EquipSocketName);

	LastCombatTime = GetWorld()->GetTimeSeconds();
}

void USKActionComponent::Multicast_SetWeaponAnimData_Implementation(USKWeaponAnimData* NewWeaponAnimData)
{
	CurrentWeaponAnimData = NewWeaponAnimData;
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;
	
	Char->GetMesh()->SetAnimInstanceClass(CurrentWeaponAnimData->AnimInstance);
	
}

void USKActionComponent::Server_SetMovementInfo_Implementation(const FVector2D NewInputVector, const EMoveDirection NewMoveDirection)
{
	CurrentInputVector = NewInputVector;
	CurrentMoveDirection = NewMoveDirection;

	SetMoveDirection();
}

void USKActionComponent::OnRep_OnMoveDirectionChange()
{
	SetMoveDirection();
}

void USKActionComponent::SetMoveDirection()
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;
	
	USKPlayerAnimInstance* PlayerAnimInstance = Cast<USKPlayerAnimInstance>(Char->GetMesh()->GetAnimInstance());
	if (PlayerAnimInstance)
	{
		PlayerAnimInstance->CurrentMoveDirection = CurrentMoveDirection;
	}
}


FRotator USKActionComponent::GetDodgeRotator() const
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return FRotator();
	
	ASKPlayerController* PC = Cast<ASKPlayerController>(Char->GetController());
	if (!PC) return FRotator();

	const FRotator ControllerRot = PC->GetControlRotation();
	FVector2D InputVector = CurrentInputVector;
	FRotator TempRot = ControllerRot;
	TempRot.Roll = 0.f;
	TempRot.Pitch = 0.f;

	const FVector TempForward = FRotationMatrix(TempRot).GetUnitAxis(EAxis::X);
	const FVector TempRight = FRotationMatrix(TempRot).GetUnitAxis(EAxis::Y);

	const FVector TargetVector = TempForward * InputVector.X + TempRight * InputVector.Y;
	const FRotator TargetRot = TargetVector.GetSafeNormal().Rotation();
	UE_LOG(LogTemp, Warning, TEXT("TargetRotation, %s"), *TargetRot.ToString())
	return TargetRot;
}

bool USKActionComponent::CheckDoubleTap()
{
	bool bIsDoubleTap = false;
	constexpr float TabDelay = 0.2f;
	
	if (bIsGateOpen)
	{
		bIsDoubleTap = true;
	}

	bIsGateOpen = true;
	GetWorld()->GetTimerManager().ClearTimer(GateTimerHandle);
	
	GetWorld()->GetTimerManager().SetTimer(
		GateTimerHandle,
		this,
		&USKActionComponent::CloseGate,
		TabDelay,
		false);
	
	return bIsDoubleTap;
}

void USKActionComponent::CloseGate()
{
	bIsGateOpen = false;
}

void USKActionComponent::Server_ExecuteDodge_Implementation(FName DodgeTag)
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;

	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayTagContainer StepTag;
	StepTag.AddTag(FGameplayTag::RequestGameplayTag(DodgeTag));
	ASC->TryActivateAbilitiesByTag(StepTag);
}

void USKActionComponent::TryDodge()
{
	bool bIsEvade = CheckDoubleTap();
	FName DodgeTag;
	if (!bIsEvade)
	{
		DodgeTag = FName("State.Action.Dodge.Step");
	}
	else
	{
		DodgeTag = FName("State.Action.Dodge.Evade");
	}
	
	Server_ExecuteDodge(DodgeTag);
}