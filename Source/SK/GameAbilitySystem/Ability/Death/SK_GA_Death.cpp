#include "SK_GA_Death.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

USK_GA_Death::USK_GA_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void USK_GA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

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
	}
	GetWorld()->GetTimerManager().ClearTimer(ActionComponent->AutoUnEquippedTimerHandle);
	
	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponData Is Null"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("WeaponData, %s"), *WeaponAnimData->GetName())
	
	UAnimMontage* DeathMontage = WeaponAnimData->DeathMontages;
	ASKPlayerController* PC = Cast<ASKPlayerController>(Char->GetController());
	
	if (Char->HasAuthority())
	{
		FTimerHandle RespawnTimer;
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, [PC](){PC->RequestRespawn();}, RespawnDelay, false);
	}
	if (Char->IsLocallyControlled())
	{
		FTimerHandle DeathUITimer;
		GetWorld()->GetTimerManager().SetTimer(DeathUITimer, this, &USK_GA_Death::ShowDeathUI, DeathUIDelay, false);
	}
	
	if (DeathMontage)
	{
		Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Death"), DeathMontage);
		PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
		PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAnimTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Death montage is null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
}

void USK_GA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	UE_LOG(LogTemp, Warning, TEXT("End %s Ability, %s"), *GetName(),
		   Char->HasAuthority() ? TEXT("Server") : TEXT("Client"));
	
	// Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void USK_GA_Death::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Death::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Death::ShowDeathUI()
{
	if (UWorld* InnerWorld = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(InnerWorld))
		{
			FSlotVisibilityMessage SlotMessage;
			SlotMessage.LayoutTag = TAG_UI_Layout_InGame;
			SlotMessage.SlotTags.AddTag(TAG_UI_Slot_CharacterDeath);
			SlotMessage.bVisible = true;

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible, SlotMessage);
		}
	}
}
