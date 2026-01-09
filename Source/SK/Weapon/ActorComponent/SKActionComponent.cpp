#include "SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Controller/SKPlayerController.h"
#include "Animation/SKPlayerAnimInstance.h"
#include "Component/BattleComponent.h"
#include "GameAbilitySystem/Ability/Unequip/SK_GA_Unequip.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/SKPlayerState.h"
#include "Weapon/SKWeaponData.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Components/CapsuleComponent.h"
#include "GameData/SKGameConstant.h"
#include "Item/Bonfire/SKStool.h"
#include "Item/Bonfire/SKBonfire.h"
#include "Components/WidgetComponent.h"
#include "Utility/SKNativeGameplayTags.h"

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
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Character) return;
	UBattleComponent* BattleComponent = Character->GetBattleComponent();
	if (!BattleComponent) return;
	BattleComponent->CurrentWeaponData = NewWeaponData;
}

void USKActionComponent::CheckAutoUnEquipped()
{
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Character || !Character->HasAuthority()) return;
	
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!ASC) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (AnimInstance->IsAnyMontagePlaying())
	{
		return;
	}

	float Now = GetWorld()->GetTimeSeconds();

	if (ASC->HasMatchingGameplayTag(TAG_State_Condition_Equip))
	{
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

void USKActionComponent::SetBonfireWidgetVisibility(ASKInteractableBase* TargetActor, bool bOnWidget)
{
	Server_SetIgnoreCollision(!bOnWidget);
	if (!bOnWidget) InteractedStool = TargetActor;
	else InteractedStool->bCanInteract = true;
	
	ASKStool* Stool = Cast<ASKStool>(InteractedStool? InteractedStool : TargetActor);
	if (Stool)
	{
		ASKBonfire* Bonfire = Stool->OwnerBonfire;
		if (Bonfire)
		{
			TSet<UWidgetComponent*>& DetectWidgets = Bonfire->DetectWidgets;
			for (auto* Widget : DetectWidgets)
			{
				Widget->SetVisibility(bOnWidget);
			}
		}
	}
	if (bOnWidget) InteractedStool = nullptr;
}

void USKActionComponent::OnRep_IgnoreCollision()
{
	ApplyCollisionSetting();
}

void USKActionComponent::ApplyCollisionSetting()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;
	
	UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
	
	Capsule->SetCollisionResponseToChannel(
		SKConstant::ECC_Interactable,
		bIgnoreCollision ? ECR_Ignore : ECR_Block
	);
}

void USKActionComponent::OnOwnerPossessed()
{
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Character || !Character->HasAuthority()) return;
	
	GetWorld()->GetTimerManager().SetTimer(AutoUnEquippedTimerHandle, this, &USKActionComponent::CheckAutoUnEquipped, 0.33f, true);

	ASKPlayerState* PlayerState = Cast<ASKPlayerState>(Character->GetPlayerState());
	if (IsValid(PlayerState))
	{
		// 죽었을 때 리스폰
		if (!PlayerState->bIsFirstSpawned)
		{
			// 무기 데이터 설정
			UE_LOG(LogTemp, Warning, TEXT("Respawn or Level Change"))
			const FWeaponDataRow* WeaponDataRow = PlayerState->GetWeaponDataRow();
			if (!WeaponDataRow)
			{
				UE_LOG(LogTemp, Error, TEXT("WeaponDataRow is null (PossessedBy timing)"));
				return;
			}
			Multicast_SetWeaponAnimData(WeaponDataRow->WeaponAnimData);
			Multicast_SetWeaponData(WeaponDataRow->WeaponData);
			// 장착 태그 부여
			ApplyEquipGE(false);
			return;
		}
		PlayerState->bIsFirstSpawned = false;
	}
	UE_LOG(LogTemp, Warning, TEXT("First Spawn"))
	
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!ASC) return;
	// 초기 태그 설정
	if (!ASC->HasMatchingGameplayTag(TAG_Weapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("First Spawn and Not Equipped"))
		ASC->AddLooseGameplayTag(TAG_Weapon_Unarmed);
		if (IsValid(PlayerState))
		{
			PlayerState->SetCurWeaponTag(TAG_Weapon_Unarmed);
		}
	}
	else
	{
		// 무기 장착한 채 레벨 전환 시에만
		UE_LOG(LogTemp, Warning, TEXT("First Spawn and Equipped, Weapon Name : %s"), *PlayerState->WeaponActors[0]->GetName())
		UE_LOG(LogTemp, Warning, TEXT("Player State Weapon Tag: %s"), *PlayerState->GetWeaponTag().GetTagName().ToString());
		const FWeaponDataRow* WeaponDataRow = PlayerState->GetWeaponDataRow();
		if (!WeaponDataRow)
		{
			UE_LOG(LogTemp, Error, TEXT("WeaponDataRow is null (PossessedBy timing)"));
			return;
		}
		Multicast_SetWeaponAnimData(WeaponDataRow->WeaponAnimData);
		Multicast_SetWeaponData(WeaponDataRow->WeaponData);
		WeaponActors = PlayerState->WeaponActors;
	}

	const FWeaponDataRow* WeaponDataRow = PlayerState->GetWeaponDataRow();
	if (!WeaponDataRow) return;
	
	CurrentWeaponAnimData = WeaponDataRow->WeaponAnimData;
	
	Character->GetMesh()->SetAnimInstanceClass(CurrentWeaponAnimData->AnimInstance);
}

void USKActionComponent::AttachWeapon(const TArray<FName> SocketNames)
{
	if (WeaponActors.IsEmpty() || SocketNames.IsEmpty()) return;

	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Character) return;

	USkeletalMeshComponent* Mesh = Character->GetMesh();
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

void USKActionComponent::OnCombatAction(bool bIsEquip)
{
	UE_LOG(LogTemp, Display, TEXT("OnCombatAction"));

	ApplyEquipGE(bIsEquip);

	if (bIsEquip)
	{
		AttachWeapon(GetWeaponAnimData()->EquipSocketName);
		LastCombatTime = GetWorld()->GetTimeSeconds();
	}
	else
	{
		AttachWeapon(GetWeaponAnimData()->UnequipSocketName);
	}
}

void USKActionComponent::ApplyEquipGE(bool bIsEquip)
{
	ASKPlayerCharacter* Charcter = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Charcter) return;

	UAbilitySystemComponent* ASC = Charcter->GetAbilitySystemComponent();
	if (!ASC) return;

	USKWeaponAnimData* AnimData = GetWeaponAnimData();
	if (!AnimData) return;
		
	if (AnimData->UnequipGE && AnimData->EquipGE)
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->UnequipGE, ASC, 1);
		ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->EquipGE, ASC, 1);
				
		FGameplayEffectSpecHandle EquipGESpecHandle = ASC->MakeOutgoingSpec(bIsEquip ? AnimData->EquipGE : AnimData->UnequipGE, 1.f, ASC->MakeEffectContext());
		if (EquipGESpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*EquipGESpecHandle.Data.Get());
		}
	}
}

void USKActionComponent::Multicast_SetWeaponAnimData_Implementation(USKWeaponAnimData* NewWeaponAnimData)
{
	CurrentWeaponAnimData = NewWeaponAnimData;
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Character) return;
	
	Character->GetMesh()->SetAnimInstanceClass(CurrentWeaponAnimData->AnimInstance);
	
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
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Character) return;
	
	USKPlayerAnimInstance* PlayerAnimInstance = Cast<USKPlayerAnimInstance>(Character->GetMesh()->GetAnimInstance());
	if (PlayerAnimInstance)
	{
		PlayerAnimInstance->CurrentMoveDirection = CurrentMoveDirection;
	}
}


FRotator USKActionComponent::GetDodgeRotator() const
{
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Character) return FRotator();
	
	ASKPlayerController* PC = Cast<ASKPlayerController>(Character->GetController());
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

void USKActionComponent::Server_ExecuteDodge_Implementation(bool bIsEvade)
{
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Character) return;

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayTagContainer StepTags;
	StepTags.AddTag(TAG_State_Action_Dodge_Step);

	FGameplayTagContainer EvadeTags;
	EvadeTags.AddTag(TAG_State_Action_Dodge_Evade);
	
	if (!bIsEvade)
	{
		if (ASC->HasMatchingGameplayTag(TAG_State_Condition_StepBlocked))
		{
			return;
		}

		ASC->TryActivateAbilitiesByTag(StepTags);
	}
	else
	{
		if (ASC->HasMatchingGameplayTag(TAG_State_Condition_EvadeBlocked))
		{
			return;
		}
		
		if (ASC->HasMatchingGameplayTag(TAG_State_Condition_StepBlocked))
		{
			ASC->CancelAbilities(&StepTags, nullptr);
			ASC->TryActivateAbilitiesByTag(EvadeTags);
		}
		
		ASC->CancelAbilities(&EvadeTags, nullptr);
		ASC->TryActivateAbilitiesByTag(EvadeTags);
	}
}

void USKActionComponent::TryDodge()
{
	bool bIsEvade = CheckDoubleTap();
	Server_ExecuteDodge(bIsEvade);
}