// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "SKCharacterBase.generated.h"


class UAbilitySystemComponent;
class USKAttributeSet;
UCLASS()
class SK_API ASKCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASKCharacterBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
#pragma region GAS
	//충돌이나 속도,운동관련
	void BaseSetting();
	void InitASCFromPlayerState();

	void OnSpeedAttributeChanged(const FOnAttributeChangeData& Data);

	/** PlayerState가 가진 ASC를 참조 (소유 X) */
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	USKAttributeSet* AttributeSet;

#pragma endregion

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
