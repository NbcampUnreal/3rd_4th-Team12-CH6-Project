// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "InventoryComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Object/EquipmentInstance.h"
#include "PlayerState/SKPlayerState.h"
#include "Weapon/SKWeaponData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UEquipmentComponent::EquipItem(const FGuid& UniqueID, const int32 ItemID)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}
	FGuid TempID = UniqueID;
	
	if (!Inventory)
	{
		return false;
	}
		

	UEquipmentInstance* Instance = Inventory->GetEquipmentInstance(UniqueID);
	if (!Instance)
	{
		return false;
	}
	
	USKEquipmentItemData* ItemData = Cast<USKEquipmentItemData>(Inventory->GetItemDataByID(ItemID));
	if (!ItemData)
	{
		return false;
	}
		

	EEquipmentSlotType Slot = ItemData->SlotType;

	if (Equipments.Contains(Slot))
	{
		UnequipItem(Slot);
	}
	
	FEquipmentSlotData& SlotData = Equipments.FindOrAdd(Slot);
	SlotData.ItemID = ItemID;
	SlotData.UniqueID = TempID;
	SlotData.EquipmentInstance = Instance;
	SlotData.EquipmentInstance->SpawnEquipmentActors(GetEquipPawn(), ItemData->ActorsToSpawnData);
	
	ApplyEquipmentEffect(ItemData, Instance);
	
	Client_UpdateEquipment(Slot, SlotData);

	FGameplayTag NewWeaponTag = SlotData.EquipmentInstance->EquipTag;

	ASKPlayerState* PlayerState = Cast<ASKPlayerState>(GetOwner());
	if (IsValid(PlayerState))
	{
		PlayerState->SetCurWeaponTag(NewWeaponTag);
		
		ASKPlayerController* PC = Cast<ASKPlayerController>(PlayerState->GetPlayerController());
		if (PC)
		{
			PC->SetLockedTarget(nullptr);
			PC->SetLockOnState(false);
		}
	}
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetEquipPawn());
	if (!Character) return false;
	USKActionComponent* ActionComponent = Character->GetActionComponent();
	if (!ActionComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionComponent is null"));
		return false;
	}
	
	const FWeaponDataRow* WeaponDataRow = PlayerState->GetWeaponDataRow();
	if (!WeaponDataRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent: EquipItem Not WeaponDataRow"));
		return false;
	}
	ActionComponent->Multicast_SetWeaponAnimData(WeaponDataRow->WeaponAnimData);
	ActionComponent->Multicast_SetWeaponData(WeaponDataRow->WeaponData);
	ActionComponent->WeaponActors = SlotData.EquipmentInstance->GetSpawnedActors();

	UAbilitySystemComponent* ASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	if (ASC)
	{
		FGameplayTag WeaponTag = SlotData.EquipmentInstance->EquipTag;
		FGameplayTag UnarmedTag = FGameplayTag::RequestGameplayTag(TEXT("Weapon.Unarmed"));
		ASC->RemoveLooseGameplayTag(UnarmedTag);
		ASC->AddLooseGameplayTag(WeaponTag);
	}
	
	return true;
}

bool UEquipmentComponent::UnequipItem(EEquipmentSlotType Slot)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}
	
	if (!Equipments.Contains(Slot))
		return false;

	FEquipmentSlotData& SlotData = Equipments[Slot];

	UAbilitySystemComponent* ASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC) return false;
	
	if (!SlotData.EquipmentInstance) return false;
	
	FGameplayTag WeaponTag = SlotData.EquipmentInstance->EquipTag;
	FGameplayTag UnarmedTag = FGameplayTag::RequestGameplayTag(TEXT("Weapon.Unarmed"));
	ASC->RemoveLooseGameplayTag(WeaponTag);
	ASC->AddLooseGameplayTag(UnarmedTag);
	
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetEquipPawn());
	if (!Character) return false;
	
	USKActionComponent* ActionComponent = Character->GetActionComponent();
	if (!ActionComponent) return false;

	ASKPlayerState* PlayerState = Cast<ASKPlayerState>(GetOwner());
	if (IsValid(PlayerState))
	{
		PlayerState->SetCurWeaponTag(WeaponTag);
	}
	const FWeaponDataRow* WeaponDataRow = PlayerState->GetWeaponDataRow();
	if (!WeaponDataRow) return false;
	
	ActionComponent->Multicast_SetWeaponAnimData(WeaponDataRow->WeaponAnimData);

	if (SlotData.EquipmentInstance)
	{
		SlotData.EquipmentInstance->DestroyEquipmentActors();
	}
	
	RemoveEquipmentEffect(SlotData);

	FEquipmentSlotData EmptyData;
	Equipments.Add(Slot, EmptyData);

	// 클라 UI 업데이트
	Client_UpdateEquipment(Slot, EmptyData);
	
	return true;
}

const FEquipmentSlotData* UEquipmentComponent::GetEquipment(EEquipmentSlotType Slot)
{

	if (const FEquipmentSlotData* SlotData = Equipments.Find(Slot))
	{
		return SlotData;
	}
	
	return nullptr;
}


void UEquipmentComponent::CopyTo(UEquipmentComponent* Target)
{
	if (!Target) return;

	auto CopySlot = [&](EEquipmentSlotType SlotType)
	{
		FEquipmentSlotData* SourceSlotData = Equipments.Find(SlotType);
		if (!SourceSlotData) return;

		// FindOrAdd 사용: Key 없으면 생성, 참조 반환
		FEquipmentSlotData& TargetSlotData = Target->Equipments.FindOrAdd(SlotType);
		TargetSlotData.UniqueID = SourceSlotData->UniqueID;
		TargetSlotData.ItemID   = SourceSlotData->ItemID;
		TargetSlotData.EquipmentInstance = SourceSlotData->EquipmentInstance;
	};

	// 슬롯별로 호출
	CopySlot(EEquipmentSlotType::Weapon);
	CopySlot(EEquipmentSlotType::Helmet);
	CopySlot(EEquipmentSlotType::Chest);
	CopySlot(EEquipmentSlotType::Leg);
	CopySlot(EEquipmentSlotType::Boots);
	CopySlot(EEquipmentSlotType::Accessory1);
	CopySlot(EEquipmentSlotType::Accessory2);
}

void UEquipmentComponent::ReSpawnWeapon()
{
	if (Equipments.Num() <= 0)
		return;

	if (!Inventory)
	{
		Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
	}
	
	auto LogEquipResult = [&](EEquipmentSlotType SlotType)
	{
		if (FEquipmentSlotData* Slot = Equipments.Find(SlotType))
		{
			const bool bResult = EquipItem(Slot->UniqueID, Slot->ItemID);
		}
	};

	// 슬롯별 호출
	LogEquipResult(EEquipmentSlotType::Weapon);
	LogEquipResult(EEquipmentSlotType::Helmet);
	LogEquipResult(EEquipmentSlotType::Chest);
	LogEquipResult(EEquipmentSlotType::Leg);
	LogEquipResult(EEquipmentSlotType::Boots);
	LogEquipResult(EEquipmentSlotType::Accessory1);
	LogEquipResult(EEquipmentSlotType::Accessory2);
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();

	if (Equipments.Num() > 0)
	{
		return;
	}
		
	
	UEnum* EnumPtr = StaticEnum<EEquipmentSlotType>();
	if (!EnumPtr) return;

	int32 Count = EnumPtr->NumEnums();

	for (uint8 i = 0; i < static_cast<uint8>(EEquipmentSlotType::Count); i++)
	{
		EEquipmentSlotType SlotType = static_cast<EEquipmentSlotType>(i);

		if (SlotType == EEquipmentSlotType::None)
			continue;

		Equipments.Add(SlotType, FEquipmentSlotData{});
	}
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentComponent, Inventory);
}

void UEquipmentComponent::ApplyEquipmentEffect(USKEquipmentItemData* ItemData, UEquipmentInstance* Instance)
{
	if (!ItemData || !Instance)
		return;

	if (!GetOwner()->HasAuthority())
		return;

	UAbilitySystemComponent* ASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
	{
		return;
	}

	// Ability 부여
	if (ItemData->EquipmentGA)
	{
		FGameplayAbilitySpec Spec(ItemData->EquipmentGA, 1);
		Instance->GrantedAbilityHandle = ASC->GiveAbility(Spec);
	}

	// Effect 부여
	if (ItemData->EquipmentGE)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		const int32 EffectLevel = 1;
 
		// GameplayEffectSpec 생성
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ItemData->EquipmentGE, EffectLevel, Context);
 
		if (SpecHandle.IsValid())
		{
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			if (Spec)
			{
				// Stats 맵 순회 (EEquipmentStat, FEquipData)
				for (const TPair<EEquipmentStat, FEquipData>& StatPair : ItemData->Stats)
				{
					const FEquipData& EquipData = StatPair.Value;
					// FGameplayTag(EquipData.DataTag)와 Magnitude를 SetByCaller로 전달
					Spec->SetSetByCallerMagnitude(EquipData.DataTag, EquipData.Magnitude);
				}
 
				// Spec을 자기 자신에게 적용 후 핸들 저장
				Instance->GrantedEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
			}
		}
	}
}

void UEquipmentComponent::RemoveEquipmentEffect(FEquipmentSlotData& SlotData)
{
	if (!SlotData.EquipmentInstance)
		return;

	if (!GetOwner()->HasAuthority())
		return;
	
	UAbilitySystemComponent* ASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
	{
		return;
	}

	// Ability 제거
	if (SlotData.EquipmentInstance->GrantedAbilityHandle.IsValid())
	{
		ASC->ClearAbility(SlotData.EquipmentInstance->GrantedAbilityHandle);
	}

	// Effect 제거
	if (SlotData.EquipmentInstance->GrantedEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(SlotData.EquipmentInstance->GrantedEffectHandle);
	}
}

APawn* UEquipmentComponent::GetEquipPawn()
{
	APlayerState* PlayerState = Cast<APlayerState>(GetOwner());
	if (!PlayerState)
	{
		return nullptr;
	}
 
	APlayerController* PC = PlayerState->GetPlayerController();
	if (!PC)
	{
		return nullptr;
	}
 
	return PC->GetPawn();
}

void UEquipmentComponent::ServerEquipItem_Implementation(const FGuid& UniqueID, const int32 ItemID)
{
	EquipItem(UniqueID, ItemID);
}
bool UEquipmentComponent::ServerEquipItem_Validate(const FGuid& UniqueID, const int32 ItemID)
{
	return true;
}
void UEquipmentComponent::ServerUnequipItem_Implementation(EEquipmentSlotType Slot)
{
	UnequipItem(Slot);
}
bool UEquipmentComponent::ServerUnequipItem_Validate(EEquipmentSlotType Slot)
{
	return true;
}

void UEquipmentComponent::Client_UpdateEquipment_Implementation(EEquipmentSlotType Slot, const FEquipmentSlotData& NewData)
{
	Equipments.Add(Slot, NewData);
}
