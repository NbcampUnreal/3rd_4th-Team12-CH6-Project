// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "SKPlayerDataAsset.h"
#include "SKCharacterBase.generated.h"

UCLASS()
class SK_API ASKCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASKCharacterBase();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;
	
#pragma region GAS_DATA
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "SK|GAS")
	TSoftObjectPtr<USKPlayerDataAsset> CharacterData;
#pragma	endregion

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
#pragma region GAS
	//충돌이나 속도,운동관련
	void BaseSetting();
	//DA ->AttributeSet 받아오는 함수
	void SetDAPlayerStat();

	// GAS 핵심 컴포넌트들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SK|GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SK|GAS")
	class USKAttributeSet* AttributeSet;
	
#pragma endregion
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
