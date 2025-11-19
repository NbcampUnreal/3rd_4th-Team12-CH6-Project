// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Inventory/Data/EquipmentItemData.h"
#include "EquipmentComponent.generated.h"


class UInventoryComponent;
class UEquipmentInstance;

USTRUCT(BlueprintType)
struct FEquipmentSlotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid UniqueID;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEquipmentInstance> EquipmentInstance;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SK_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	UFUNCTION(BlueprintCallable, Category="Equipment")
	bool EquipItem(const FGuid& UniqueID, const int32 ItemID);

	UFUNCTION(BlueprintCallable, Category="Equipment")
	bool UnequipItem(EEquipmentSlotType Slot);

	UFUNCTION(BlueprintCallable, Category="Equipment")
	UEquipmentInstance* GetEquipment(EEquipmentSlotType Slot) const;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerEquipItem(const FGuid& UniqueID, const int32 ItemID);
 
	// 서버 RPC 함수 - ID, Count로 아이템 제거 요청
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerUnequipItem(EEquipmentSlotType Slot);
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
	UPROPERTY(VisibleAnywhere, Category="Equipment")
	TMap<EEquipmentSlotType, FEquipmentSlotData> Equipments;

	UPROPERTY(Replicated)
	TObjectPtr<UInventoryComponent> Inventory;
	
	void ApplyEquipmentEffect(UEquipmentItemData* ItemData, UEquipmentInstance* Instance);
	void RemoveEquipmentEffect(FEquipmentSlotData& SlotData);

	UFUNCTION(Client, Reliable)
	void Client_UpdateEquipment(EEquipmentSlotType Slot, const FEquipmentSlotData& NewData);
	
};
