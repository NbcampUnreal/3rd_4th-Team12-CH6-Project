// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SKAttributeSet.generated.h"


// 매크로 - Getter/Setter 자동 생성
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients: 
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_SixParams(FSKAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);
/**
 * 
 */
UCLASS()
class SK_API USKAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	USKAttributeSet();

	// 능력치목록
	/*
	 *
	Health,MaxHealth,
	Stamina,MaxStatmina
	Heat,MaxHeat,
	Exp,Level,Gold
	Attack,Armor,
	Poiseness(강인도 100에 가까울수록 공격으로부터 회복빠름)
	
	*/
#pragma region AttributeSet
	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Speed)
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, MaxHealth)

	// Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, MaxStamina)

	// Heat
	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Heat)
	FGameplayAttributeData Heat;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Heat)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_MaxHeat)
	FGameplayAttributeData MaxHeat;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, MaxHeat)

	// Experience & Level
	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Exp)
	FGameplayAttributeData Exp;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Exp)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Level)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Gold)

	// Combat Stats
	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Attack)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Attack)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Armor)

	//강인도
	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Poise)
	FGameplayAttributeData Poise;
	ATTRIBUTE_ACCESSORS(USKAttributeSet, Poise)


#pragma endregion
	
	mutable FSKAttributeEvent OnHealthChanged;

	mutable FSKAttributeEvent OnMaxHealthChanged;

	mutable FSKAttributeEvent OnStaminaChanged;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
#pragma region Replicated
	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldSpeed);

	
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);


	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	// Stamina
	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	// Heat
	UFUNCTION()
	virtual void OnRep_Heat(const FGameplayAttributeData& OldHeat);

	UFUNCTION()
	virtual void OnRep_MaxHeat(const FGameplayAttributeData& OldMaxHeat);

	// Experience & Level
	UFUNCTION()
	virtual void OnRep_Exp(const FGameplayAttributeData& OldExp);

	UFUNCTION()
	virtual void OnRep_Level(const FGameplayAttributeData& OldLevel);

	UFUNCTION()
	virtual void OnRep_Gold(const FGameplayAttributeData& OldGold);

	// Combat Stats
	UFUNCTION()
	virtual void OnRep_Attack(const FGameplayAttributeData& OldAttack);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	virtual void OnRep_Poise(const FGameplayAttributeData& OldPoise);

#pragma endregion
};
