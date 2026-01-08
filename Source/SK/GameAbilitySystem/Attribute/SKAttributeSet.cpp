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
	InitSpeed(500.f);
	InitSprintWeight(1.3f);
}

void USKAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Heat, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, MaxHeat, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Gold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Exp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Poise, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAttributeSet, SprintWeight, COND_None, REPNOTIFY_Always);
}

void USKAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);


#pragma region Stat
	//Health
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float Delta = Data.EvaluatedData.Magnitude;

		if (Delta < 0.f)
		{
			OnDamageTaken.Broadcast(-Delta);
		}
		
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		const float DeltaValue = Data.EvaluatedData.Magnitude;

		// 현재 체력에 변화량만큼 더하기
		SetHealth(FMath::Clamp(GetHealth() + DeltaValue, 0.0f, GetMaxHealth()));
	}

	//Stamina

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		//변화량
		const float DeltaValue = Data.EvaluatedData.Magnitude;

		SetStamina(FMath::Clamp(GetStamina() + DeltaValue, 0.0f, GetMaxStamina()));
	}
	//Heat
	if (Data.EvaluatedData.Attribute == GetHeatAttribute())
	{
		SetHeat(FMath::Clamp(GetHeat(), 0.0f, GetMaxHeat()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHeatAttribute())
	{
		//변화량
		const float DeltaValue = Data.EvaluatedData.Magnitude;

		SetHeat(FMath::Clamp(GetHeat() + DeltaValue, 0.0f, GetMaxHeat()));
	}
#pragma endregion
}

void USKAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

#pragma region Stat


#pragma endregion

	//Sever
	if (Attribute == GetHealthAttribute())
	{
		if (NewValue < OldValue)
		{
			const float Damage = OldValue - NewValue;
			OnDamageTaken.Broadcast(Damage);
		}
		
		UE_LOG(LogTemp, Log, TEXT("Health1 Changed: OldValue: %f | NewValue: %f"), OldValue, NewValue);
		// FString DebugMsg = FString::Printf(TEXT("Player Health: %.2f"), NewValue);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DebugMsg);
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
	else if (Attribute == GetHeatAttribute())
	{
		OnHeatChanged.Broadcast(
			nullptr,
			nullptr,
			nullptr,
			OldValue - NewValue,
			OldValue,
			NewValue
		);
	}
	else if (Attribute == GetMaxHeatAttribute())
	{
		OnMaxHeatChanged.Broadcast(
			nullptr,
			nullptr,
			nullptr,
			OldValue - NewValue,
			OldValue,
			NewValue
		);
	}
	else if (Attribute == GetGoldAttribute())
	{
		OnGoldChanged.Broadcast(
			nullptr,
			nullptr,
			nullptr,
			NewValue - OldValue,
			OldValue,
			NewValue
		);
	}

	// 죽음 처리
	if (GetHealth() <= 0.01f)
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (ASC)
		{
			FGameplayTag DeathTag = FGameplayTag::RequestGameplayTag(FName("State.Condition.Death"));

			if (ASC->HasMatchingGameplayTag(DeathTag))
			{
				return;
			}

			ASC->AddLooseGameplayTag(DeathTag);

			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(DeathTag));
		}
	}
}

void USKAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//스태미나 
	if (Attribute == GetStaminaAttribute())
	{
		//MaxStatmina 넘지않게 하기
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}

	//스태미나 
	if (Attribute == GetHeatAttribute())
	{
		//MaxStatmina 넘지않게 하기
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHeat());
	}
}

void USKAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Speed, OldSpeed);
}

void USKAttributeSet::OnRep_SprintWeight(const FGameplayAttributeData& OldSprintWeight)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, SprintWeight, OldSprintWeight);
}


void USKAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Health, OldHealth);

	OnHealthChanged.Broadcast(
		nullptr,
		nullptr,
		nullptr,
		GetHealth() - OldHealth.GetCurrentValue(),
		OldHealth.GetCurrentValue(),
		GetHealth()
	);
}

void USKAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, MaxHealth, OldMaxHealth);
}

void USKAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAttributeSet, Stamina, OldStamina);
	OnStaminaChanged.Broadcast(
		nullptr,
		nullptr,
		nullptr,
		GetStamina() - OldStamina.GetCurrentValue(),
		OldStamina.GetCurrentValue(),
		GetStamina()
	);
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

	OnGoldChanged.Broadcast(
		nullptr,
		nullptr,
		nullptr,
		GetGold() - OldGold.GetCurrentValue(),
		OldGold.GetCurrentValue(),
		GetGold()
	);
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

void USKAttributeSet::HandleDamageTaken(
	float Damage,
	const FGameplayEffectModCallbackData& Data)
{
	OnDamageTaken.Broadcast(Damage);
}
