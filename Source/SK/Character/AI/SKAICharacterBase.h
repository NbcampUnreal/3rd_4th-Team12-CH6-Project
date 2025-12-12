#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameData/StaticData/MonsterDataTable.h"
#include "SKAICharacterBase.generated.h"

class UBoxComponent;
class UMotionWarpingComponent;
class USKAIAttributeSet;
class USKAIDataAsset;
class UStateTree;

UCLASS()
class SK_API ASKAICharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component|Combat")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Animation")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Montages")
	TMap<FName, TObjectPtr<UAnimMontage>> Montages;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTree> StateTreeAsset;

	int32 CurrentMeleeIndex = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Index")
	int32 MaxMeleeIndex = 0;

private:
	FVector StartLocation;

	FTimerHandle OverlayTimerHandle;

public:
	ASKAICharacterBase();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void InitializeAttributeSetAndAbilitiesFromDataAsset();

	void SendEventToASC(AActor* LocalInstigator, AActor* LocalTargetActor, FGameplayTag EventTag) const;
	
	TMap<FName, TObjectPtr<UAnimMontage>> GetMontages() const;
	
	TObjectPtr<UStateTree> GetStateTreeAsset() const;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentMeleeIndex() const;

	UFUNCTION(BlueprintCallable)
	int32 GetMaxMeleeIndex() const;

	UFUNCTION(BlueprintCallable)
	void SetMeleeIndex(int32 NewMeleeIndex);

	FVector GetStartLocation() const;

	int32 GetDropTableID() const;

	FMonsterData GetMonsterData() const;

	//오버레이머티리얼 Set함수-이준식
	void SetOverlayMaterial(UMaterialInterface* OverlayMat, float Duration = 10.f);
	void ClearOverlayMaterial();
protected:
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
	
	void ApplyStaticMonsterStats();
};
