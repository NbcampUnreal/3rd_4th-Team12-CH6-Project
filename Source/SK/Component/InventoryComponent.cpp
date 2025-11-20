// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/ActorChannel.h"
#include "GameData/StaticData/ItemDataTable.h"
#include "GameFramework/PlayerState.h"
#include "Item/Inventory/Data/SKConsumableItemData.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "Net/UnrealNetwork.h"
#include "Object/EquipmentInstance.h"
#include "Utility/StaticDataSubsystem.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


bool UInventoryComponent::AddItemByIDAndCount(const int32& ItemID, int32 Count)
{
	UE_LOG(LogTemp, Warning, TEXT("[AddItemByIDAndCount] ItemData  %d"), ItemID);
	
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}
	
	if (Count <= 0) return false;

	USKInventoryItemData* ItemData = GetItemDataByID(ItemID);
	if (!ItemData)
	{
		
		return false;
	}

	if (ItemData->bIsStackable)
	{
		bool bAddItem = false;
		for (FInventorySlot& Slot : InventorySlots)
		{
			if (Slot.ItemID == ItemID)
			{
				Slot.Count += Count;
				bAddItem = true;
				break;
			}
		}
		if (!bAddItem)
		{
			InventorySlots.Add(FInventorySlot{ItemID, Count, FGuid()});
		}
		
		return true;
	}
	else if (ItemData->InventoryType == EInventoryItemType::Equipment)
	{
		FInventorySlot NewSlot(ItemID, 1, FGuid());
			
		NewSlot.UniqueID = FGuid::NewGuid();
		
		UEquipmentInstance* EquipInstance = NewObject<UEquipmentInstance>(this);
		FEquipmentInstanceSlot NewEquipInstance(NewSlot.UniqueID, EquipInstance);
		EquipmentInstances.Add(NewEquipInstance);
			
		InventorySlots.Add(NewSlot);
		
		return true;
	}
	else
	{
		FInventorySlot NewSlot(ItemID, 1, FGuid());
			
		InventorySlots.Add(NewSlot);
	}

	return false;
}

bool UInventoryComponent::RemoveItemByIDAndCount(const int32& ItemID, int32 Count)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByIDAndCount] Called on client! ItemID: %d"), ItemID);
		return false;
	}
	
	if (Count <= 0) return false;

	USKInventoryItemData* ItemData = GetItemDataByID(ItemID);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByIDAndCount] ItemData not found for ItemID: %d"), ItemID);
		return false;
	}

	// 장비는 UniqueID로만 제거할 수 있다.
	if (ItemData->InventoryType == EInventoryItemType::Equipment)
	{
		UE_LOG(LogTemp, Error, TEXT("[RemoveItemByIDAndCount] Equipment items must be removed using UniqueID!"));
		return false;
	}
	
	if (ItemData->bIsStackable)
	{
		for (int32 i = 0; i < InventorySlots.Num(); ++i)
		{
			FInventorySlot& Slot = InventorySlots[i];
			if (Slot.ItemID == ItemID)
			{
				if (Slot.Count < Count)
				{
					UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByIDAndCount] Not enough stack items to remove."));
					return false;
				}

				Slot.Count -= Count;

				if (Slot.Count <= 0)
					InventorySlots.RemoveAt(i);

				return true;
			}
		}

		// 아이템 없음
		return false;
	}

	int32 NeedRemove = Count;

	for (int32 i = InventorySlots.Num() - 1; i >= 0 && NeedRemove > 0; --i)
	{
		if (InventorySlots[i].ItemID == ItemID)
		{
			InventorySlots.RemoveAt(i);
			NeedRemove--;
		}
	}

	if (NeedRemove > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByIDAndCount] Not enough non-stack items to remove."));
		return false;
	}

	return true;
}

bool UInventoryComponent::RemoveItemByUniqueID(const FGuid& UniqueID)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByUniqueID] Called on client!"));
		return false;
	}
	
	if (!UniqueID.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByUniqueID] Invalid UniqueID."));
		return false;
	}

	bool bRemoved = false;

	for (int32 i = 0; i < InventorySlots.Num(); ++i)
	{
		if (InventorySlots[i].UniqueID == UniqueID)
		{
			InventorySlots.RemoveAt(i);
			bRemoved = true;
			break;
		}
	}
	
	EquipmentInstances.RemoveAll(
		[&](const FEquipmentInstanceSlot& Slot)
		{
			return Slot.UniqueID == UniqueID;
		}
	);

	return bRemoved;
}

bool UInventoryComponent::UseItemByID(int32 UseItemID)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UseItemByID] Called on client!"));
		return false;
	}
	
	FInventorySlot* Slot = InventorySlots.FindByPredicate(
		[&](const FInventorySlot& S)
		{
			return S.ItemID == UseItemID;
		}
	);

	if (!Slot)
	{
		UE_LOG(LogTemp, Warning, TEXT("UseItemByID: ItemID %d not found in inventory"), UseItemID);
		return false;
	}
	
	// 2) 데이터 가져오기
	USKInventoryItemData* UseItemDataBase = GetItemDataByID(UseItemID);
	if (!UseItemDataBase)
	{
		UE_LOG(LogTemp, Error, TEXT("UseItemByID: Slot ItemData is null (ItemID: %d)"), UseItemID);
		return false;
	}

	if (UseItemDataBase->InventoryType != EInventoryItemType::Consumable)
	{
		return false;
	}
	
	// 3) 소모 아이템인지 캐스트
	USKConsumableItemData* ConsumableData = Cast<USKConsumableItemData>(UseItemDataBase);
	if (!ConsumableData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UseItemByID: ItemID %d is not Consumable"), UseItemID);
		return false;
	}

	// 4) GA 이름 로그 출력
	if (ConsumableData->ConsumableGA)
	{
		UE_LOG(LogTemp, Log, TEXT("Consumable GA: %s"), 
			*ConsumableData->ConsumableGA->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Consumable GA is null for ItemID %d"), UseItemID);
	}

	bool bActivated = false;
	
	// ★ 실제 사용 처리 (예: GE 적용 또는 GA Activation 등) 여기 추가 ★
	if (ConsumableData->ConsumableGA)
	{
		// ASC 찾기
		UAbilitySystemComponent* ASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
		if (!ASC)
		{
			return false;
		}

		bActivated = ItemAbilityCheckAndActive(ASC, ConsumableData->ConsumableGA);
	}
	
	if (bActivated)
	{
		RemoveItemByIDAndCount(UseItemID, 1);

		return true;
	}

	return false;
}

bool UInventoryComponent::ItemAbilityCheckAndActive(UAbilitySystemComponent* ASC,
	TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!ASC || !AbilityClass)
	{
		return false;
	}
 
	FGameplayAbilitySpecHandle FoundHandle;
 
	// 1. 이미 부여된 어빌리티가 있는지 찾기
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->GetClass() == AbilityClass)
		{
			FoundHandle = Spec.Handle;
			break;
		}
	}
 
	bool bActivated = false;
 
	if (FoundHandle.IsValid())
	{
		// 어빌리티가 이미 부여된 경우 바로 실행
		bActivated = ASC->TryActivateAbility(FoundHandle);
	}
	else
	{
		// 부여되지 않은 경우 → 부여 후 실행 → 제거
		FGameplayAbilitySpec Spec(AbilityClass, 1, 0);
		FGameplayAbilitySpecHandle NewHandle = ASC->GiveAbility(Spec);
 
		if (!NewHandle.IsValid())
		{
			return false;
		}
 
		bActivated = ASC->TryActivateAbility(NewHandle);
 
		// 실행 후 바로 제거
		ASC->ClearAbility(NewHandle);
	}
 
	return bActivated;
}

int32 UInventoryComponent::GetItemCountByID(const int32& ItemID)
{
	int32 TotalCount = 0;
 
	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.ItemID == ItemID)
		{
			TotalCount += Slot.Count;
		}
	}
 
	return TotalCount;
}

TArray<FInventorySlot> UInventoryComponent::GetAllItems() const
{
	return InventorySlots;
}

TArray<FInventorySlot> UInventoryComponent::GetItemsByType(EInventoryItemType ItemType) const
{
	TArray<FInventorySlot> Result;
	for (const FInventorySlot& Slot : InventorySlots)
	{
		USKInventoryItemData* ItemData = GetItemDataByID(Slot.ItemID);
		if (ItemData && ItemData->InventoryType == ItemType)
		{
			Result.Add(Slot);
		}
	}
	return Result;
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (const FEquipmentInstanceSlot& Slot : EquipmentInstances)
	{
		UEquipmentInstance* EquipInst = Slot.EquipmentInstance.Get();
		
		if (IsValid(EquipInst) && !EquipInst->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed) && EquipInst->IsSupportedForNetworking())
		{
			bWroteSomething |= Channel->ReplicateSubobject(EquipInst, *Bunch, *RepFlags);
		}
		else if (EquipInst)
		{
			UE_LOG(LogTemp, Warning, TEXT("Skipping replication of invalid or destroyed EquipmentInstance: %s"), *EquipInst->GetName());
		}
	}
	
	return bWroteSomething;
}

void UInventoryComponent::ServerAddItem_Implementation(const int32& ItemID, int32 Count)
{
	AddItemByIDAndCount(ItemID, Count);
}
bool UInventoryComponent::ServerAddItem_Validate(const int32& ItemID, int32 Count)
{
	return true;
}
 
// 서버 RPC RemoveItemByID
void UInventoryComponent::ServerRemoveItemByID_Implementation(const int32& ItemID, int32 Count)
{
	RemoveItemByIDAndCount(ItemID, Count);
}
bool UInventoryComponent::ServerRemoveItemByID_Validate(const int32& ItemID, int32 Count)
{
	return true;
}
 
// 서버 RPC RemoveItemByUniqueID
void UInventoryComponent::ServerRemoveItemByUniqueID_Implementation(const FGuid& UniqueID)
{
	RemoveItemByUniqueID(UniqueID);
}
bool UInventoryComponent::ServerRemoveItemByUniqueID_Validate(const FGuid& UniqueID)
{
	return true;
}

void UInventoryComponent::ServerUseItemByID_Implementation(const int32& UseItemID)
{
	UseItemByID(UseItemID);
}
bool UInventoryComponent::ServerUseItemByID_Validate(const int32& UseItemID)
{
	return true;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventorySlots);
	DOREPLIFETIME(UInventoryComponent, EquipmentInstances);
}

USKInventoryItemData* UInventoryComponent::GetItemDataByID(const int32& ItemID) const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return nullptr;
	
	UStaticDataSubsystem* SDS = GI->GetSubsystem<UStaticDataSubsystem>();
	if(!SDS) return nullptr;

	const FItemData* ItemData = SDS->GetData<FItemData>(ItemID);

	if (ItemData->InventoryItemDataAsset.IsValid())
	{
		return ItemData->InventoryItemDataAsset.Get();
	}
	else if (!ItemData->InventoryItemDataAsset.ToSoftObjectPath().IsNull())
	{
		return ItemData->InventoryItemDataAsset.LoadSynchronous();
	}
	
	return nullptr;
}

UEquipmentInstance* UInventoryComponent::GetEquipmentInstance(const FGuid& UniqueID) const
{
	if (!UniqueID.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[GetEquipmentInstance] Invalid UniqueID."));
		return nullptr;
	}

	for (const FEquipmentInstanceSlot& Slot : EquipmentInstances)
	{
		if (Slot.UniqueID == UniqueID)
		{
			return Slot.EquipmentInstance;
		}
	}

	
	return nullptr;
}
