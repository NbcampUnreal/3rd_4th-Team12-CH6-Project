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

	ASKPlayerState* PlayerState = Cast<ASKPlayerState>(Char->GetPlayerState());
	if (IsValid(PlayerState))
	{
		if (!PlayerState->bIsFirstSpawned)
		{
			// 무기 데이터 설정
			const FWeaponDataRow* WeaponDataRow = PlayerState->GetWeaponDataRow();
			Multicast_SetWeaponAnimData(WeaponDataRow->WeaponAnimData);
			Multicast_SetWeaponData(WeaponDataRow->WeaponData);
			// 장착 태그 부여
			ApplyEquipGE(false);
			return;
		}
		PlayerState->bIsFirstSpawned = false;
	}

	GetWorld()->GetTimerManager().SetTimer(AutoUnEquippedTimerHandle, this, &USKActionComponent::CheckAutoUnEquipped, 0.33f, true);
	
	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (!ASC) return;
	// 초기 태그 설정
	FGameplayTag UnarmedTag = FGameplayTag::RequestGameplayTag(TEXT("Weapon.Unarmed"));
	ASC->AddLooseGameplayTag(UnarmedTag);
	
	if (IsValid(PlayerState))
	{
		PlayerState->SetCurWeaponTag(UnarmedTag);
	}
	const FWeaponDataRow* WeaponDataRow = PlayerState->GetWeaponDataRow();
	if (!WeaponDataRow) return;
	
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