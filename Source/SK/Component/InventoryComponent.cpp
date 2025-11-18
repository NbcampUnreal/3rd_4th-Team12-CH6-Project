// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameData/StaticData/ItemDataTable.h"
#include "GameFramework/PlayerState.h"
#include "Item/Inventory/Data/ConsumableItemData.h"
#include "Item/Inventory/Data/InventoryItemData.h"
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
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	
	if (Count <= 0) return false;

	UInventoryItemData* ItemData = GetItemDataByID(ItemID);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AddItemByIDAndCount] ItemData not found for %d"), ItemID);
		return false;
	}
	
	if (ItemData->bIsStackable)
	{
		for (FInventorySlot& Slot : InventorySlots)
		{
			if (Slot.ItemID == ItemID)
			{
				Slot.Count += Count;
				return true;
			}
		}
		InventorySlots.Add(FInventorySlot{ItemID, Count, FGuid()});
		return true;
	}
	else
	{
		for (int32 i = 0; i < Count; ++i)
		{
			FInventorySlot NewSlot(ItemID, 1, FGuid());
 
			if (ItemData->InventoryType == EInventoryItemType::Equipment)
			{
				NewSlot.UniqueID = FGuid::NewGuid();
				UEquipmentInstance* EquipInstance = NewObject<UEquipmentInstance>(this);

				FEquipmentInstanceSlot NewEquipInstance(NewSlot.UniqueID, EquipInstance);
				EquipmentInstances.Add(NewEquipInstance);
			}
			InventorySlots.Add(NewSlot);
		}
		return true;
	}
}

bool UInventoryComponent::RemoveItemByIDAndCount(const int32& ItemID, int32 Count)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByIDAndCount] Called on client! ItemID: %d"), ItemID);
		return false;
	}
	
	if (Count <= 0) return false;

	UInventoryItemData* ItemData = GetItemDataByID(ItemID);
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

	// --------------------------
	// 스택형 아이템
	// --------------------------
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

	// --------------------------
	// 비스택형 일반 아이템
	// --------------------------
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

	// ------------------------------------
	// 인벤토리 슬롯에서 제거
	// ------------------------------------
	for (int32 i = 0; i < InventorySlots.Num(); ++i)
	{
		if (InventorySlots[i].UniqueID == UniqueID)
		{
			InventorySlots.RemoveAt(i);
			bRemoved = true;
			break;
		}
	}

	// ------------------------------------
	// 장비 인스턴스에서 제거
	// ------------------------------------
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
	UInventoryItemData* UseItemDataBase = GetItemDataByID(UseItemID);
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
	UConsumableItemData* ConsumableData = Cast<UConsumableItemData>(UseItemDataBase);
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
		UE_LOG(LogTemp, Log, TEXT("[UseItem] Consumable GA found: %s"),
			*ConsumableData->ConsumableGA->GetName());

		// OwnerActor = 캐릭터로 변환
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
			return false;
		}

		UE_LOG(LogTemp, Log, TEXT("[UseItem] OwnerActor: %s"), *OwnerActor->GetName());

		// ASC 찾기
		UAbilitySystemComponent* ASC = OwnerActor->FindComponentByClass<UAbilitySystemComponent>();
		if (!ASC)
		{
			UE_LOG(LogTemp, Error, TEXT("[UseItem] AbilitySystemComponent is NULL on actor %s"),
				*OwnerActor->GetName());
			return false;
		}

		UE_LOG(LogTemp, Log, TEXT("[UseItem] ASC Found. Giving Ability..."));

		// Ability 부여
		FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(
			FGameplayAbilitySpec(ConsumableData->ConsumableGA, 1, 0)
		);

		if (!Handle.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("[UseItem] AbilitySpecHandle is INVALID"));
			return false;
		}

		// Ability 활성화 시도
		bActivated = ASC->TryActivateAbility(Handle);

		UE_LOG(LogTemp, Log, TEXT("[UseItem] TryActivateAbility result: %s"),
			bActivated ? TEXT("Success") : TEXT("Failed"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UseItem] Consumable GA is NULL"));
	}

	if (bActivated)
	{
		RemoveItemByIDAndCount(UseItemID, 1);

		return true;
	}
	// 5) 개수 감소 처리

	return false;
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
		UInventoryItemData* ItemData = GetItemDataByID(Slot.ItemID);
		if (ItemData && ItemData->InventoryType == ItemType)
		{
			Result.Add(Slot);
		}
	}
	return Result;
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

UInventoryItemData* UInventoryComponent::GetItemDataByID(const int32& ItemID) const
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
