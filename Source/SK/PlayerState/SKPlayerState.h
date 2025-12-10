// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Character/SKPlayerDataAsset.h"
#include "GameData/WeaponDataRow.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "SKPlayerState.generated.h"

class ASKBonfire;
class UInventoryComponent;
class UQuickSlotComponent;
class UEquipmentComponent;
class UAbilitySystemComponent;
class USKAttributeSet;
class UStaticDataSubsystem;

struct FWeaponDataRow;
struct FSKWeaponDataRow;

USTRUCT(BlueprintType)
struct FModifiedAttributeInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttribute Attribute;
	
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EGameplayModOp::Type> Op;
	
	UPROPERTY(BlueprintReadOnly)
	float Magnitude;

	UPROPERTY(BlueprintReadOnly)
	float Duration;
};

USTRUCT(BlueprintType)
struct FModifiedAttributeArray
{
	GENERATED_BODY()
 
	UPROPERTY()
	TArray<FModifiedAttributeInfo> Items;
};

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnBuffAdded,
	FActiveGameplayEffectHandle, EffectHandle,
	const FModifiedAttributeArray&, ModifiedAttributes,
	FGameplayTagContainer, BuffTags,
	float, Duration
);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnTimeChanged,
	FActiveGameplayEffectHandle, EffectHandle,
	float, NewStart,
	float, NewDuration
);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnStackChanged,
	FActiveGameplayEffectHandle, EffectHandle,
	int32, NewStack,
	int32, OldStack
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuffRemoved, const FActiveGameplayEffectHandle, EffectHandle, FModifiedAttributeArray, ModifiedAttributes);

UCLASS()
class SK_API ASKPlayerState : public APlayerState, public IGenericTeamAgentInterface
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

	/** TeamID 반환 */
	virtual FGenericTeamId GetGenericTeamId() const override { return PlayerTeamID; }
	/** 팀 갱신 함수 */
	void SetTeamFromTag(const FGameplayTag& TeamTag);
	FGenericTeamId PlayerTeamID = FGenericTeamId::NoTeam;

	void SetCurWeaponTag(FGameplayTag NewTag);

	void EquipmentComponentSetting();

#pragma region LevelSystem
	
	/** Gold 지급 */
	void AddGold(int32 Value);

	/** 서버가 레벨업 요청 처리 */
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_RequestLevelUp();

	/** 내부 레벨업 처리 */
	UFUNCTION()
	void TryLevelUp();

	/** AbilityPoint 감소 → 스탯 강화 시 사용 */
	UFUNCTION()
	void ConsumeAbilityPoint();

	UFUNCTION(BlueprintCallable)
	int32 GetGold() const { return Gold; }

	UFUNCTION(BlueprintCallable)
	int32 GetPlayerLevel() const { return Level; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetAbilityPoint() const { return AbilityPoint; }

	UFUNCTION(BlueprintCallable)
	int32 GetRequiredGoldForNextLevel() const;
	
	
	UPROPERTY(ReplicatedUsing=OnRep_Gold, BlueprintReadOnly)
	int32 Gold = 0;
	
	UPROPERTY(ReplicatedUsing=OnRep_Level, BlueprintReadOnly)
	int32 Level = 1;
	
	UPROPERTY(ReplicatedUsing=OnRep_AbilityPoint, BlueprintReadOnly)
	int32 AbilityPoint = 0;
	
#pragma endregion LevelSystem
	
	
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

	UFUNCTION(BlueprintCallable)
	FWeaponDataRow& GetWeaponData();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Data")
	TObjectPtr<UDataTable> WeaponDT;
	
	UFUNCTION(BlueprintCallable, Category="SK|Weapon")
	TArray<FName> GetTraceSocket();
	TSoftObjectPtr<UDataTable> GetWeaponDT() const;
	TSoftObjectPtr<UDataTable> GetWeaponData() const;
	
	FGameplayTag GetWeaponTag() const;


#pragma endregion

	UPROPERTY(BlueprintAssignable, Category="SK|Buff")
    FOnBuffAdded OnBuffAdded;
         
    UPROPERTY(BlueprintAssignable, Category="SK|Buff")
    FOnBuffRemoved OnBuffRemoved;

	UPROPERTY(BlueprintAssignable, Category="SK|Buff")
    FOnTimeChanged OnBuffTimeChanged;

	UPROPERTY(BlueprintAssignable, Category="SK|Buff")
	FOnStackChanged OnBuffStackChanged;
	
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

	UFUNCTION()
	void HandleGameplayEffectAdded(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
 
	UFUNCTION()
	void HandleGameplayEffectRemoved(const FActiveGameplayEffect& Effect);

	UFUNCTION()
	void HandleGameplayEffectStackChange(FActiveGameplayEffectHandle Handle, int32 NewStack, int32 OldStack);

	UFUNCTION()
	void HandleGameplayEffectTimeChange(FActiveGameplayEffectHandle Handle, float NewStartTime, float NewDuration);
	
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, FModifiedAttributeArray> ModifiedAttributeMap;


#pragma region LevelSystem
	UFUNCTION()
	void OnRep_Gold();

	UFUNCTION()
	void OnRep_Level();

	UFUNCTION()
	void OnRep_AbilityPoint();

	// StaticDataSubsystem 캐싱용
	UStaticDataSubsystem* SDS;
#pragma endregion LevelSystem
	
public:
	UPROPERTY()
	ASKBonfire* CurrentBonfire;
};
