// Fill out your copyright notice in the Description page of Project Settings.


#include "SK_GA_InteractionBase.h"

#include "Character/SKPlayerCharacter.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"
#include "Interaction/Interface/SKInteractable.h"
#include "Item/SKInteractableBase.h"
#include "Item/Openable/SKOpenableBase.h"

USK_GA_InteractionBase::USK_GA_InteractionBase()
	: CachedTargetActor(nullptr)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void USK_GA_InteractionBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
			
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Active Interaction Ability, %s"), Char->HasAuthority() ? TEXT("Server") : TEXT("Client"));

	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	ASKInteractableBase* TargetActor = InteractionComponent->GetCurrentTargetActor();
	if (!TargetActor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	CachedTargetActor = TargetActor;
	CachedInteractionData = InteractionComponent->GetInteractionData();

	TargetActor->PreExecuteInteraction(Char);
	
	// UI 처리
	UWidgetComponent* InteractionWidget = TargetActor->InteractionWidget;
	if (InteractionWidget)
	{
		InteractionComponent->Client_ToggleInteractableWidget(InteractionWidget, false);
	}
	ASKOpenableBase* OpenableTarget = Cast<ASKOpenableBase>(TargetActor);
	if (OpenableTarget)
	{
		UWidgetComponent* DetectWidget = OpenableTarget->DetectWidget;
		if (DetectWidget)
		{
			InteractionComponent->Client_ToggleInteractableWidget(DetectWidget, false);
		}
	}
}

void USK_GA_InteractionBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("End Interaction Ability"));

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char) return;
	
	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent) return;
	
	// 로컬 캐릭터, 재상호작용 가능한 것만 UI 처리
	if (Char->IsLocallyControlled())
	{
		if (CachedTargetActor)
		{
			if (CachedTargetActor->ObjectType == EObjectType::Fireplace)
			{
				ASKOpenableBase* OpenableTarget = Cast<ASKOpenableBase>(CachedTargetActor);
				if (!OpenableTarget) return;
	
				UWidgetComponent* DetectWidget = OpenableTarget->DetectWidget;
				if (DetectWidget)
				{
					InteractionComponent->Client_ToggleInteractableWidget(DetectWidget, true);
				}
			}
		}
	}

	InteractionComponent->SetInteractionData(FSKInteractionData());
	CachedTargetActor = nullptr;
	CachedInteractionData = FSKInteractionData();
}