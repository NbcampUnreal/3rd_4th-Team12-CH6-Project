#include "SK_GA_Dodge.h"

#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Component/SKCombatComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

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

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Active %s Ability, %s"), *GetName(),
	       Char->HasAuthority() ? TEXT("Server") : TEXT("Client"));

	USKActionComponent* ActionComponent = Char->GetActionComponent();
	if (!ActionComponent)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 회피 별 값 세팅
	PreActivateDodge(ActionComponent);

	const FName SectionName = SetDodgeDirection(Char, ActionComponent);

	// 몽타주 재생
	if (DodgeMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("Dodge"), DodgeMontage, 1.f, SectionName);
		PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
		PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAnimTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't Have Dodge Montage!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}


	if (USKCombatComponent* Combat = Char->GetCombatComponent())
	{
		Combat->ResetComboState();
	}
}

void USK_GA_Dodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                              bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("End Ability, %s"), *GetName());
}

void USK_GA_Dodge::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Dodge::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Dodge::PreActivateDodge(USKActionComponent* ActionComponent)
{
}

FName USK_GA_Dodge::SetDodgeDirection(ASKPlayerCharacter* PlayerCharacter, USKActionComponent* ActionComponent)
{
	const FRotator DodgeRotation = ActionComponent->GetDodgeRotator();
	FRotator AdjustedRot = DodgeRotation;

	FName Direction;
	
	// 락온 일 때
	if (PlayerCharacter->bIsLockedOn)
	{
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
