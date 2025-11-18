// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/QuickSlotComponent.h"

#include "InventoryComponent.h"
#include "Item/Inventory/Data/ConsumableItemData.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UQuickSlotComponent::UQuickSlotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UQuickSlotComponent::SetQuickSlot(int32 SlotIndex, int32 ItemID)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetQuickSlot] Called on client!"));
		return false;
	}
	
	if (!QuickSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetQuickSlot: 유효하지 않은 슬롯 인덱스 %d"), SlotIndex);
		return false;
	}
 
	UInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("SetQuickSlot: 인벤토리 컴포넌트를 찾을 수 없습니다."));
		return false;
	}
 
	int32 ItemCount = Inventory->GetItemCountByID(ItemID);

	if (ItemCount <= 0)
	{
		return false;
	}
	
	UInventoryItemData* ItemData = Inventory->GetItemDataByID(ItemID);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("SetQuickSlot: 아이템 데이터가 존재하지 않습니다. ItemID=%d"), ItemID);
		return false;
	}

	UConsumableItemData* ConsumItemData = Cast<UConsumableItemData>(ItemData);
	
	int32 SlotCount = FMath::Min(ItemCount, ConsumItemData->QuickSlotSize);
 
	QuickSlots[SlotIndex].ItemID = ItemID;
	QuickSlots[SlotIndex].Count = SlotCount;
 
	UE_LOG(LogTemp, Log, TEXT("SetQuickSlot: 슬롯 %d에 아이템 %d, 개수 %d 설정"), SlotIndex, ItemID, SlotCount);
 
	return true;
}

bool UQuickSlotComponent::ClearQuickSlot(int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ClearQuickSlot] Called on client!"));
		return false;
	}
 
	if (!QuickSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ClearQuickSlot] Invalid SlotIndex: %d"), SlotIndex);
		return false;
	}
 
	QuickSlots[SlotIndex].ItemID = -1;
	QuickSlots[SlotIndex].Count = 0;
 
	UE_LOG(LogTemp, Log, TEXT("[ClearQuickSlot] Slot %d cleared."), SlotIndex);
 
	// 서버에서 값 변경 시 복제되어 클라이언트 동기화됨
	return true;
}

bool UQuickSlotComponent::UseQuickSlot(int32 SlotIndex)
{
	return true;
}


// Called when the game starts
void UQuickSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UQuickSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UQuickSlotComponent, QuickSlots);
}

void UQuickSlotComponent::ServerSetQuickSlot_Implementation(int32 SlotIndex, int32 ItemID)
{
	SetQuickSlot(SlotIndex, ItemID);
}
bool UQuickSlotComponent::ServerSetQuickSlot_Validate(int32 SlotIndex, int32 ItemID)
{
	return true;
}

void UQuickSlotComponent::ServerClearQuickSlot_Implementation(int32 SlotIndex)
{
	ClearQuickSlot(SlotIndex);
}
bool UQuickSlotComponent::ServerClearQuickSlot_Validate(int32 SlotIndex)
{
	return true;
}