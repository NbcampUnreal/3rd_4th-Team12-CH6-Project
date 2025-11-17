#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SKAIAttributeSet.generated.h"

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

UCLASS()
class SK_API USKAIAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	USKAIAttributeSet();

	// stat
	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USKAIAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USKAIAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Attack)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(USKAIAttributeSet, Attack)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(USKAIAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Poise)
	FGameplayAttributeData Poise;
	ATTRIBUTE_ACCESSORS(USKAIAttributeSet, Poise)

	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(USKAIAttributeSet, Speed)
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributeset", ReplicatedUsing = OnRep_SprintWeight)
	FGameplayAttributeData SprintWeight;
	ATTRIBUTE_ACCESSORS(USKAIAttributeSet, SprintWeight)

	// delegate
	mutable FSKAttributeEvent OnCurrentHealthChanged;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldCurrentHealth);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	
	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldAttack);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	void OnRep_Poise(const FGameplayAttributeData& OldPoise);

	UFUNCTION()
	void OnRep_Speed(const FGameplayAttributeData& OldSpeed);
	
	UFUNCTION()
	void OnRep_SprintWeight(const FGameplayAttributeData& OldSprintWeight);
};
