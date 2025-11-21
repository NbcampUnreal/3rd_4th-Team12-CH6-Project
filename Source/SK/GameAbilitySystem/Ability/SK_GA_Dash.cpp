// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_Dash.h"

#include "AbilitySystemComponent.h"
#include "Character/SKCharacterBase.h"
#include "Character/SKPlayerCharacter.h"
#include "GameInstance/SKGameInstance.h"
#include "Utility/SKBGMSubSystem.h"

USK_GA_Dash::USK_GA_Dash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	DashActiveTag = FGameplayTag::RequestGameplayTag(FName("Ability.Dash.Active"));
	ActivationBlockedTags.AddTag(DashActiveTag);

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Dash")));
}

void USK_GA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//코스트
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}


	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character)
	{
		// CommitAbility 후에는 반드시 CancelAbility를 호출해야 합니다.
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}


	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		ASC->AddLooseGameplayTag(DashActiveTag);
	}

	ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(ActorInfo->AvatarActor.Get());

	SKCharacter->ResetComboState();

	if (Character)
	{
		const FVector DashDir = Character->GetActorForwardVector();

		// 대시 속도 = 거리 / 시간
		FVector LaunchVel = DashDir * (DashDistance / FMath::Max(0.001f, DashDuration));
		Character->LaunchCharacter(LaunchVel, true, true); // 대시 속도 조정
		// 필요하면 기존 속도나 MaxWalkSpeed 저장
		// PrevMaxWalkSpeed = MoveComp->MaxWalkSpeed;

		UGameInstance* GI = GetWorld()->GetGameInstance();
		USKGameInstance* SKGI = Cast<USKGameInstance>(GI);
		if (USKBGMSubSystem* BGM = SKGI->GetSubsystem<USKBGMSubSystem>())
		{
			FGameplayTag DashSFXTag = FGameplayTag::RequestGameplayTag(FName("Sound.SFX.ATK_00"));
			FVector PlayerLocation = Character->GetActorLocation();
			BGM->PlaySoundByTag(DashSFXTag, PlayerLocation);
		}
		// 대시가 끝나면 어빌리티 종료
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(
			DashTimerHandle,
			[this, Handle, ActorInfo, ActivationInfo]()
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			},
			DashDuration,
			false
		);
	}
}

void USK_GA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                             const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                             bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(DashActiveTag);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
