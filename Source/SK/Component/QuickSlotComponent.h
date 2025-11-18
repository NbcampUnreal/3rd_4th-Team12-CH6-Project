// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuickSlotComponent.generated.h"

USTRUCT(BlueprintType)
struct FQuickSlot
{
	GENERATED_BODY()

	// 슬롯에 들어있는 아이템 ID (-1이면 빈 슬롯)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID = -1;

	// 슬롯에 들어있는 아이템 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SK_API UQuickSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuickSlotComponent();

	UFUNCTION(BlueprintCallable, Category="Inventory|QuickSlot")
	bool SetQuickSlot(int32 SlotIndex, int32 ItemID);

	UFUNCTION(BlueprintCallable, Category="Inventory|QuickSlot")
	bool ClearQuickSlot(int32 SlotIndex);
	
	// 퀵슬롯 사용 (아이템 ID 기준)
	UFUNCTION(BlueprintCallable, Category="Inventory|QuickSlot")
	bool UseQuickSlot(int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerSetQuickSlot(int32 SlotIndex, int32 ItemID);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerClearQuickSlot(int32 SlotIndex);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<FQuickSlot> QuickSlots = { FQuickSlot(), FQuickSlot(), FQuickSlot() };
		
};
