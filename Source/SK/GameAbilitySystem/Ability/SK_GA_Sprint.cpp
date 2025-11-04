// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_Sprint.h"

#include "Character/SKPlayerCharacter.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"

USK_GA_Sprint::USK_GA_Sprint()
{
}

void USK_GA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
		return;


	// 스태미나 소비용 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &USK_GA_Sprint::ConsumeStamina, 0.2f, true);
	Character->SetSprinting(true);
}

void USK_GA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                               bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);

	if (ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(ActorInfo->AvatarActor.Get()))
	{
		Character->SetSprinting(false);
	}
}

void USK_GA_Sprint::ConsumeStamina()
{
	ASKCharacterBase* Character = Cast<ASKCharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character) return;

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

	if (!ASC)
		return;

	// GE 클래스 지정
	if (!SprintStaminaCostEffect)
		return;

	// 스태미나가 0 이하라면 종료
	const float CurrentStamina = ASC->GetNumericAttribute(USKAttributeSet::GetStaminaAttribute());
	if (CurrentStamina <= 0.f)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 스태미나 코스트 GE 적용
	FGameplayEffectSpecHandle SpecHandle = ASC->
		MakeOutgoingSpec(SprintStaminaCostEffect, 1.f, ASC->MakeEffectContext());
	if (SpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}
