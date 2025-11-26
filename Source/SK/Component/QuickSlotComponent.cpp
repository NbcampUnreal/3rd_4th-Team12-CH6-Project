// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/QuickSlotComponent.h"

#include "AbilitySystemComponent.h"
#include "InventoryComponent.h"
#include "GameFramework/PlayerState.h"
#include "Item/Inventory/Data/SKConsumableItemData.h"
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
	
	if (QuickSlots[SlotIndex].ItemID != -1)
	{
		USKInventoryItemData* PrevItem = Inventory->GetItemDataByID(QuickSlots[SlotIndex].ItemID);
		if (PrevItem)
		{
			if (USKConsumableItemData* PrevConsum = Cast<USKConsumableItemData>(PrevItem))
			{
				if (PrevConsum->ConsumableGA)
				{
					UAbilitySystemComponent* ASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();

					if (ASC)
					{
						ASC->ClearAbility(QuickSlots[SlotIndex].GrantedAbilityHandle);
						QuickSlots[SlotIndex].GrantedAbilityHandle = FGameplayAbilitySpecHandle();
					}
				}
			}
		}
	}
	
	int32 ItemCount = Inventory->GetItemCountByID(ItemID);

	if (ItemCount <= 0)
	{
		return false;
	}
	
	USKInventoryItemData* ItemData = Inventory->GetItemDataByID(ItemID);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("SetQuickSlot: 아이템 데이터가 존재하지 않습니다. ItemID=%d"), ItemID);
		return false;
	}

	USKConsumableItemData* ConsumItemData = Cast<USKConsumableItemData>(ItemData);
	
	int32 SlotCount = FMath::Min(ItemCount, ConsumItemData->QuickSlotSize);
 
	QuickSlots[SlotIndex].ItemID = ItemID;
	QuickSlots[SlotIndex].Count = SlotCount;

	if (ConsumItemData && ConsumItemData->ConsumableGA)
	{
		UAbilitySystemComponent* ASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();

		if (ASC)
		{
			FGameplayAbilitySpec Spec(ConsumItemData->ConsumableGA, 1, (int32)SlotIndex, this);
			QuickSlots[SlotIndex].GrantedAbilityHandle = ASC->GiveAbility(Spec);
		}
	}
	return true;
}

bool UQuickSlotComponent::ClearQuickSlot(int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}
 
	if (!QuickSlots.IsValidIndex(SlotIndex))
	{
		return false;
	}
	
	if (QuickSlots[SlotIndex].GrantedAbilityHandle.IsValid())
	{
		UAbilitySystemComponent* ASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();

		if (ASC)
		{
			ASC->ClearAbility(QuickSlots[SlotIndex].GrantedAbilityHandle);
		}

		QuickSlots[SlotIndex].GrantedAbilityHandle = FGameplayAbilitySpecHandle();
	}
	
	QuickSlots[SlotIndex].ItemID = -1;
	QuickSlots[SlotIndex].Count = 0;
 
	// 서버에서 값 변경 시 복제되어 클라이언트 동기화됨
	return true;
}

bool UQuickSlotComponent::UseQuickSlot(int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return false;

	// 슬롯 인덱스 체크
	if (!QuickSlots.IsValidIndex(SlotIndex))
		return false;

	FQuickSlot& Slot = QuickSlots[SlotIndex];

	// Count 없으면 사용 불가
	if (Slot.Count <= 0 || Slot.ItemID == -1)
		return false;

	// 인벤토리 가져오기
	UInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!Inventory)
		return false;

	// 슬롯의 아이템 데이터
	USKInventoryItemData* ItemData = Inventory->GetItemDataByID(Slot.ItemID);
	if (!ItemData)
		return false;

	USKConsumableItemData* ConsumData = Cast<USKConsumableItemData>(ItemData);
	if (!ConsumData || !ConsumData->ConsumableGA)
		return false;
	
	// ASC 가져오기
	UAbilitySystemComponent* ASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
		return false;

	// ────────────────────────────────────────────────
	// 1) 어빌리티 실행 시도
	// ────────────────────────────────────────────────
	bool bActivated = false;

	bActivated = ASC->TryActivateAbility(QuickSlots[SlotIndex].GrantedAbilityHandle);

	if (!bActivated)
	{
		return false;
	}

	Slot.Count--;

	Inventory->RemoveItemByIDAndCount(Slot.ItemID, 1);

	OnQuickSlotsUpdated.Broadcast();
	
	return true;
}

void UQuickSlotComponent::RefreshQuickSlots()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;

	UInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		return;
	}

	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		FQuickSlot& Slot = QuickSlots[i];

		// 빈 슬롯은 스킵
		if (Slot.ItemID == -1)
			continue;

		// 인벤토리에서 현재 남은 개수 확인
		int32 CurrentCount = Inventory->GetItemCountByID(Slot.ItemID);

		if (CurrentCount <= 0)
		{
			continue;
		}
		
		USKInventoryItemData* ItemData = Inventory->GetItemDataByID(Slot.ItemID);
		if (!ItemData)
		{
			continue;
		}

		USKConsumableItemData* ConsumData = Cast<USKConsumableItemData>(ItemData);
		if (!ConsumData)
		{
			continue;
		}

		int32 NewCount = FMath::Min(CurrentCount, ConsumData->QuickSlotSize);
		
		Slot.Count = NewCount;
	}
}

TArray<FQuickSlot> UQuickSlotComponent::GetQuickSlots()
{
	return QuickSlots;
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

void UQuickSlotComponent::OnRep_QuickSlots()
{
	OnQuickSlotsUpdated.Broadcast();
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

void UQuickSlotComponent::ServerUseQuickSlot_Implementation(int32 SlotIndex)
{
	UseQuickSlot(SlotIndex);
}
bool UQuickSlotComponent::ServerUseQuickSlot_Validate(int32 SlotIndex)
{
	return true;
}

void UQuickSlotComponent::ServerRefreshQuickSlots_Implementation()
{
	RefreshQuickSlots();
}
bool UQuickSlotComponent::ServerRefreshQuickSlots_Validate()
{
	return true;
}