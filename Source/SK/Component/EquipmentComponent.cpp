// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "InventoryComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Object/EquipmentInstance.h"

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

	if (!Inventory)
		return false;

	UEquipmentInstance* Instance = Inventory->GetEquipmentInstance(UniqueID);
	if (!Instance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipItem] Instance not found for %s"), *UniqueID.ToString());
		return false;
	}

	UEquipmentItemData* ItemData = Cast<UEquipmentItemData>(Inventory->GetItemDataByID(ItemID));
	if (!ItemData)
		return false;

	EEquipmentSlotType Slot = ItemData->SlotType;

	if (Equipments.Contains(Slot))
	{
		UnequipItem(Slot);
	}

	FEquipmentSlotData& SlotData = Equipments.FindOrAdd(Slot);
	SlotData.ItemID = ItemID;
	SlotData.UniqueID = UniqueID;
	SlotData.EquipmentInstance = Instance;

	ApplyEquipmentEffect(ItemData, Instance);

	Client_UpdateEquipment(Slot, SlotData);
	
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

	RemoveEquipmentEffect(SlotData);

	FEquipmentSlotData EmptyData;
	Equipments.Add(Slot, EmptyData);

	// 클라 UI 업데이트
	Client_UpdateEquipment(Slot, EmptyData);
	
	return true;
}

UEquipmentInstance* UEquipmentComponent::GetEquipment(EEquipmentSlotType Slot) const
{

	if (const FEquipmentSlotData* SlotData = Equipments.Find(Slot))
	{
		return SlotData->EquipmentInstance;
	}
	
	return nullptr;
}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentComponent, Inventory);
}

void UEquipmentComponent::ApplyEquipmentEffect(UEquipmentItemData* ItemData, UEquipmentInstance* Instance)
{
	if (!ItemData || !Instance)
		return;

	if (!GetOwner()->HasAuthority())
		return;

	AActor* OwnerActor = nullptr;

	// 1) PlayerState → PlayerController 찾기
	APlayerState* PS = Cast<APlayerState>(GetOwner());
	if (PS)
	{
		APlayerController* PC = PS->GetPlayerController();
		if (PC)
		{
			OwnerActor = PC->GetPawn();   // 최종 캐릭터
		}
	}

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[UseItem] OwnerActor is NULL (Failed to get Character)"));
		return;
	}
	
	UAbilitySystemComponent* ASC = OwnerActor->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC) return;

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
		Instance->GrantedEffectHandle =
			ASC->ApplyGameplayEffectToSelf(ItemData->EquipmentGE->GetDefaultObject<UGameplayEffect>(), 1, Context);
	}
}

void UEquipmentComponent::RemoveEquipmentEffect(FEquipmentSlotData& SlotData)
{
	if (!SlotData.EquipmentInstance)
		return;

	if (!GetOwner()->HasAuthority())
		return;

	AActor* OwnerActor = nullptr;

	// 1) PlayerState → PlayerController 찾기
	APlayerState* PS = Cast<APlayerState>(GetOwner());
	if (PS)
	{
		APlayerController* PC = PS->GetPlayerController();
		if (PC)
		{
			OwnerActor = PC->GetPawn();   // 최종 캐릭터
		}
	}

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[UseItem] OwnerActor is NULL (Failed to get Character)"));
		return;
	}
	
	UAbilitySystemComponent* ASC = OwnerActor->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC) return;

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
