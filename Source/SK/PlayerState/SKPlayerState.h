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


USTRUCT(BlueprintType)
struct FSKRepComboState
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag WeaponTag;

	UPROPERTY()
	FGameplayTag AttackTypeTag;

	UPROPERTY()
	int32 ComboIndex = 0;
};


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

	UFUNCTION(BlueprintCallable, Category="SK|Weapon")
	TArray<FName> GetTraceSocket();
	const UDataTable* GetWeaponDT() const;
	int32 GetMaxComobo(bool bLeft = true);
	void SetDAPlayerStat();
	void SetWeaponTag(FGameplayTag WeaponTag);

	FGameplayTag GetWeapontTag() const;
	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	void OnRep_ComboState();


	int32 GetComboIndex() const;
	UFUNCTION(Server, Reliable)
	void Server_IncreaseComboIndex(bool bLeft = true);

	UFUNCTION(Server, Reliable)
	void Server_ResetComboIndex(int32 NewIndex);

#pragma endregion

protected:
#pragma region GAS
	
	UPROPERTY(ReplicatedUsing=OnRep_ComboState)
	FSKRepComboState RepComboState;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponTag,editAnywhere, BlueprintReadWrite, Category="SK|Battle")
	FGameplayTag CurrentWeaponTag;
	//Tag별 소켓 정보 DT
	UPROPERTY(EditAnywhere, Category="SK|Weapon")
	UDataTable* CurrentWeaponDT;
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
