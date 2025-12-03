
#include "SK_GA_Dodge.h"

#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Controller/SKPlayerController.h"

USK_GA_Dodge::USK_GA_Dodge()
	: DodgeMontage(nullptr)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void USK_GA_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); return; }
	
	UE_LOG(LogTemp, Warning, TEXT("Active %s Ability, %s"), *GetName(), Char->HasAuthority() ? TEXT("Server") : TEXT("Client"));

	USKActionComponent* ActionComponent = Char->GetActionComponent();
	if (!ActionComponent) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); return; }

	// 캐릭터 회전
	Char->SetActorRotation(ActionComponent->GetDodgeRotator());

	// 몽타주 섹션
	FName SectionName;
	ASKPlayerController* PC = Cast<ASKPlayerController>(Char->GetController());
	if (!PC) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); return; }

	if (PC->CurrentInputVector.IsNearlyZero())
	{
		SectionName = "Backward";
	}
	else
	{
		SectionName = "Forward";
	}
	
	// 몽타주 재생
	SetAnimMontage(ActionComponent);
	
	if (DodgeMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Dodge"), DodgeMontage, 1.f, SectionName);
		PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
		PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAnimTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't Have Dodge Montage!"));
	}
	
}

void USK_GA_Dodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("End Ability, %s"), *GetName());

}

void USK_GA_Dodge::SetAnimMontage(USKActionComponent* ActionComponent)
{
}

void USK_GA_Dodge::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Dodge::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}