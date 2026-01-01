#include "SK_GA_Dodge.h"

#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Utility/SKNativeGameplayTags.h"

USK_GA_Dodge::USK_GA_Dodge()
	: DodgeMontage(nullptr)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void USK_GA_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Active %s Ability, %s"), *GetName(),
	       Character->HasAuthority() ? TEXT("Server") : TEXT("Client"));

	USKActionComponent* ActionComponent = Character->GetActionComponent();
	if (!ActionComponent)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	

	// 회피 별 값 세팅
	PreActivateDodge(ActionComponent);

	const FName SectionName = SetDodgeDirection(Character, ActionComponent);

	// 몽타주 재생
	if (DodgeMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("Dodge"), DodgeMontage, 1.f, SectionName);
		PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
		PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAnimTask->OnCancelled.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAnimTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't Have Dodge Montage!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}



}

void USK_GA_Dodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                              bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("End Ability, %s"), *GetName());
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(TAG_State_Condition_StepBlocked);
		ASC->RemoveLooseGameplayTag(TAG_State_Condition_EvadeBlocked);
	}
}

void USK_GA_Dodge::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void USK_GA_Dodge::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Dodge::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USK_GA_Dodge::PreActivateDodge(USKActionComponent* ActionComponent)
{
}

FName USK_GA_Dodge::SetDodgeDirection(ASKPlayerCharacter* PlayerCharacter, USKActionComponent* ActionComponent)
{
	const FRotator DodgeRotation = ActionComponent->GetDodgeRotator();
	FRotator AdjustedRot = DodgeRotation;

	FName Direction;

	APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
	ASKPlayerController* SK_PC = Cast<ASKPlayerController>(PC);
	// 락온 일 때
	if (SK_PC->GetIsLockedOn())
	{
		// 입력 없을 시 뒤로
		if (ActionComponent->CurrentInputVector.IsNearlyZero())
		{
			Direction = "Backward";
			return Direction;
		}
		EMoveDirection MoveDirection = ActionComponent->CurrentMoveDirection;
		switch (MoveDirection)
		{
		case (EMoveDirection::None):
		case (EMoveDirection::Forward):
			Direction = "Forward";
			break;
		case (EMoveDirection::Backward):
			Direction = "Backward";
			break;
		case (EMoveDirection::Left):
			Direction = "Left";
			break;
		case (EMoveDirection::Right):
			Direction = "Right";
			break;
		case (EMoveDirection::ForwardLeft):
			AdjustedRot.Yaw += 90.0f;
			PlayerCharacter->SetActorRotation(AdjustedRot);
			Direction = "Left";
			break;
		case (EMoveDirection::ForwardRight):
			AdjustedRot.Yaw += -90.0f;
			PlayerCharacter->SetActorRotation(AdjustedRot);
			Direction = "Right";
			break;
		case (EMoveDirection::BackwardLeft):
			AdjustedRot.Yaw += 90.0f;
			PlayerCharacter->SetActorRotation(AdjustedRot);
			Direction = "Left";
			break;
		case (EMoveDirection::BackwardRight):
			AdjustedRot.Yaw += -90.0f;
			PlayerCharacter->SetActorRotation(AdjustedRot);
			Direction = "Right";
			break;
		}
	}
	else
	{
		// 입력 없을 시 뒤로
		if (ActionComponent->CurrentInputVector.IsNearlyZero())
		{
			Direction = "Backward";
		}
		else
		{
			// 캐릭터 회전
			PlayerCharacter->SetActorRotation(DodgeRotation);
			Direction = "Forward";
		}
	}

	return Direction;
}
