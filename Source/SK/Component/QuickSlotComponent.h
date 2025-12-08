// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/ActorComponent.h"
#include "QuickSlotComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuickSlotsUpdated);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAbilitySpecHandle GrantedAbilityHandle;
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

	UFUNCTION(BlueprintCallable, Category="Inventory|QuickSlot")
	void TryUseQuickSlot(int32 SlotIndex);
	
	// 퀵슬롯 사용 (아이템 ID 기준)
	UFUNCTION(BlueprintCallable, Category="Inventory|QuickSlot")
	bool UseQuickSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory|QuickSlot")
	void RefreshQuickSlots();

	UFUNCTION(BlueprintCallable, Category="Inventory|QuickSlot")
	TArray<FQuickSlot> GetQuickSlots();
	
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerSetQuickSlot(int32 SlotIndex, int32 ItemID);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerClearQuickSlot(int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerUseQuickSlot(int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerRefreshQuickSlots();

	UFUNCTION(Client, Reliable)
	void Client_NotifyItemCooldown(int32 SlotIndex, float Cooldown);
	
	UPROPERTY(BlueprintAssignable, Category="Inventory|QuickSlot")
	FOnQuickSlotsUpdated OnQuickSlotsUpdated;

	UFUNCTION(BlueprintCallable, Category="Inventory|QuickSlot")
	void CopyTo(UQuickSlotComponent* Target);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_QuickSlots();

	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite)
	float CurrentUseItemCooldown = 0.0f;
	
	UPROPERTY(ReplicatedUsing=OnRep_QuickSlots, VisibleAnywhere, BlueprintReadOnly)
	TArray<FQuickSlot> QuickSlots = { FQuickSlot(), FQuickSlot(), FQuickSlot() };
		
};
