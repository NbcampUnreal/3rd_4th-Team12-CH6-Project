#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "SKAICharacterBase.generated.h"

class USKAIAttributeSet;
class USKAIDataAsset;

UCLASS()
class SK_API ASKAICharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Montage")
	TArray<TObjectPtr<UAnimMontage>> Montages;

private:
	FVector StartLocation;
	
public:
	ASKAICharacterBase();

	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void InitializeAttributeSetAndAbilitiesFromDataAsset();

	TArray<UAnimMontage*> GetMontages() const;

	FVector GetStartLocation() const;

protected:
	void ApplyStaticMonsterStats();
};
