// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class USKInventoryItemData;
class UGameplayAbility;
class UAbilitySystemComponent;
enum class EInventoryItemType : uint8;
class UEquipmentInstance;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid UniqueID; // 장비면 사용, 스택형이면 사용 X

	FInventorySlot() = default;
	
	FInventorySlot(int32 InItemID, int32 InCount, FGuid InUniqueID)
		: ItemID(InItemID), Count(InCount), UniqueID(InUniqueID)
	{}
};

USTRUCT(BlueprintType)
struct FEquipmentInstanceSlot
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid UniqueID;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEquipmentInstance> EquipmentInstance;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SK_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	// 서버 RPC 함수 - 아이템 추가 요청
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerAddItem(const int32& ItemID, int32 Count);
 
	// 서버 RPC 함수 - ID, Count로 아이템 제거 요청
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerRemoveItemByID(const int32& ItemID, int32 Count);
 
	// 서버 RPC 함수 - UniqueID로 아이템 제거 요청
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerRemoveItemByUniqueID(const FGuid& UniqueID);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerUseItemByID(const int32& UseItemID);

	UFUNCTION(Client, Reliable)
	void Client_NotifyItemAdded(int32 ItemID, int32 Count);
	
	// 로컬 함수들 - 서버에서만 호출
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItemByIDAndCount(const int32& ItemID, int32 Count);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemByIDAndCount(const int32& ItemID, int32 Count);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemByUniqueID(const FGuid& UniqueID);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool UseItemByID(int32 UseItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ItemAbilityCheckAndActive(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCountByID(const int32& ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	USKInventoryItemData* GetItemDataByID(const int32& ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UEquipmentInstance* GetEquipmentInstance(const FGuid& UniqueID) const;
	
	// 아이템 목록 조회 함수
	UFUNCTION(BlueprintCallable, Category="Inventory")
	TArray<FInventorySlot> GetAllItems() const;
 
	UFUNCTION(BlueprintCallable, Category="Inventory")
	TArray<FInventorySlot> GetItemsByType(EInventoryItemType ItemType) const;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CopyTo(UInventoryComponent* Target);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> InventorySlots;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<FEquipmentInstanceSlot> EquipmentInstances;
};
