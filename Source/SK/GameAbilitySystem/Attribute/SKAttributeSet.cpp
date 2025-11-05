// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectTypes.h"   // FGameplayEffectModCallbackData 포함
#include "GameplayEffectExtension.h" // 일부 확장 관련 기능

USKAttributeSet::USKAttributeSet()
{
	InitHealth(180.f);
	InitMaxHealth(200.f);
	InitStamina(50.f);
	InitMaxStamina(100.f);
	InitHeat(1.f);
	InitMaxHeat(5.f);
	InitAttack(10.f);
	InitArmor(5.f);
	InitPoise(20.f);
	InitExp(10.f);
	InitLevel(1.f);
	InitGold(100.f);
		
}

void USKAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Heat, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, MaxHeat, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Gold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Exp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Poise, COND_None, REPNOTIFY_Always);


}

void USKAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void USKAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		UE_LOG(LogTemp, Log, TEXT("Health1 Changed: OldValue: %f | NewValue: %f"), OldValue, NewValue);
		OnHealthChanged.Broadcast(
			nullptr,
			nullptr,
			nullptr,
			NewValue - OldValue,
			OldValue,
			NewValue
		);
	}
	else if (Attribute == GetStaminaAttribute())
	{
		OnStaminaChanged.Broadcast(
			nullptr,
			nullptr,
			nullptr,
			OldValue - NewValue,
			OldValue,
			NewValue
		);
	}
}

void USKAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Speed, OldSpeed);
}

void USKAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Health, OldHealth);
}

void USKAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, MaxHealth, OldMaxHealth);
}

void USKAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Stamina, OldStamina);
}

void USKAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, MaxStamina, OldMaxStamina);
}

void USKAttributeSet::OnRep_Heat(const FGameplayAttributeData& OldHeat)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Heat, OldHeat);
}

void USKAttributeSet::OnRep_MaxHeat(const FGameplayAttributeData& OldMaxHeat)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, MaxHeat, OldMaxHeat);
}

void USKAttributeSet::OnRep_Exp(const FGameplayAttributeData& OldExp)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Exp, OldExp);
}

void USKAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Level, OldLevel);
}

void USKAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Gold, OldGold);
}

void USKAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Attack, OldAttack);
}

void USKAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Armor, OldArmor);
}

void USKAttributeSet::OnRep_Poise(const FGameplayAttributeData& OldPoise)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Poise, OldPoise);
}