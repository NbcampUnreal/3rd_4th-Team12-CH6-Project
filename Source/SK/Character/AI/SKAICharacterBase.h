#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameData/StaticData/MonsterDataTable.h"
#include "SKAICharacterBase.generated.h"

class UBoxComponent;
class USphereComponent;
class USKAIAttributeSet;
class USKAIDataAsset;
class UStateTree;
class UMotionWarpingComponent;
struct FOnAttributeChangeData;

UCLASS()
class SK_API ASKAICharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component|Combat")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component|Combat")
	TObjectPtr<USphereComponent> SphereComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	USKAIAttributeSet* AttributeSet;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSoftObjectPtr<USKAIDataAsset> AIDataAsset;
	
protected:
	/** 몬스터 정적 ID (BP에서 고정 입력) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MonsterID")
	int32 MonsterID = -1;

	/** 몬스터 정적 ID */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MonsterID")
	int32 DropTableID = -1;

	/** 몬스터 정적 ID */
	const FMonsterData* MonsterData;

	FName MonsterName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Montages")
	TMap<FName, TObjectPtr<UAnimMontage>> Montages;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTree> StateTreeAsset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackIndex")
	int32 MaxMeleeIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackIndex")
	int32 MaxRushIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackIndex")
	int32 MaxJumpRushIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackIndex")
	int32 MaxFlyRushIndex = 0;
	
private:
	FVector StartLocation;

	float BackstepDistance;
	
	FTimerHandle OverlayTimerHandle;

	
public:
	ASKAICharacterBase();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void InitializeAttributeSetAndAbilitiesFromDataAsset();

	UFUNCTION(BlueprintCallable)
	void AddTag(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	void RemoveTag(FGameplayTag Tag) const;

	void SendEventToASC(AActor* LocalInstigator, AActor* LocalTargetActor, FGameplayTag EventTag) const;

	FName GetMonsterName() const;
	
	TMap<FName, TObjectPtr<UAnimMontage>> GetMontages() const;
	
	TObjectPtr<UStateTree> GetStateTreeAsset() const;

	int32 GetMaxMeleeIndex() const;

	int32 GetMaxRushIndex() const;
	
	int32 GetMaxJumpRushIndex() const;

	int32 GetMaxFlyRushIndex() const;
	
	FVector GetStartLocation() const;

	float GetBackstepDistance() const;

	int32 GetDropTableID() const;

	FMonsterData GetMonsterData() const;

	//오버레이머티리얼 Set함수-이준식
	void SetOverlayMaterial(UMaterialInterface* OverlayMat, float Duration = 10.f);
	void ClearOverlayMaterial();
	
protected:
	virtual void PostInitializeComponents() override;

	void OnHealthChanged(const FOnAttributeChangeData& Data);

	void OnStaminaChanged(const FOnAttributeChangeData& Data);

	virtual void PossessedBy(AController* NewController) override;
	
	UFUNCTION()
	void OnBoxComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void OnBoxComponentEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);	

	UFUNCTION()
	void OnSphereComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void OnSphereComponentEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);	
	
	void ApplyStaticMonsterStats();
};
