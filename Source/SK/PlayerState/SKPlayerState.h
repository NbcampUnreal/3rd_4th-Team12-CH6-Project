// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Character/SKPlayerDataAsset.h"
#include "GameData/WeaponDataRow.h"
#include "SKPlayerState.generated.h"

class UInventoryComponent;
class UQuickSlotComponent;
class UEquipmentComponent;
class UAbilitySystemComponent;
class USKAttributeSet;


struct FWeaponDataRow;
struct FSKWeaponDataRow;

UCLASS()
class SK_API ASKPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASKPlayerState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//SeamlessTravel로 인해 PlayerState가 재성성되기 때문에 이전PS에서 데이터를 가져오기 위해 사용
	virtual void CopyProperties(APlayerState* NewPlayerState) override;
	// 네트워크 복제에 필요한 함수 재정의
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma region GAS
	UFUNCTION()
	void OnRep_CurrentWeaponTag();

	DECLARE_MULTICAST_DELEGATE(FOnASCReady);
	FOnASCReady OnASCReady;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	USKAttributeSet* GetAttributeSet() const;

	void SetDAPlayerStat();
	void SetWeaponTag(FGameplayTag WeaponTag);

	const FSKWeaponDataRow* GetWeaponSocketDataRow() const;
	const FWeaponDataRow* GetWeaponDataRow() const; 
	
	UFUNCTION(BlueprintCallable, Category="SK|Weapon")
	TArray<FName> GetTraceSocket();
	TSoftObjectPtr<UDataTable> GetWeaponDT() const;
	TSoftObjectPtr<UDataTable> GetWeaponData() const;
	
	FGameplayTag GetWeaponTag() const;


#pragma endregion

protected:
#pragma region GAS


	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponTag, editAnywhere, BlueprintReadWrite, Category="SK|Battle")
	FGameplayTag CurrentWeaponTag;

	//Tag별 소켓 정보 DT
	UPROPERTY(EditAnywhere, Category="SK|Weapon")
	TSoftObjectPtr<UDataTable> WeaponSocketDT;

	UPROPERTY(EditAnywhere, Category="SK|WeaponData")
	TSoftObjectPtr<UDataTable> WeaponDataTable;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "SK|GAS")
	TSoftObjectPtr<USKPlayerDataAsset> CharacterData;
	// GAS 핵심 컴포넌트들
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "SK|GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "SK|GAS")
	class USKAttributeSet* AttributeSet;

#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SK|Inventory", Replicated)
	UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SK|QuickSlot", Replicated)
	UQuickSlotComponent* QuickSlotComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SK|Equipment", Replicated)
	UEquipmentComponent* EquipmentComponent;
};
