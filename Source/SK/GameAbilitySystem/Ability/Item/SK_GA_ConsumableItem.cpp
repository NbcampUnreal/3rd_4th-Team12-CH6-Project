// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Item/SK_GA_ConsumableItem.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Component/EquipmentComponent.h"
#include "GameData/StaticData/ItemDataTable.h"
#include "Item/Inventory/Data/SKConsumableItemData.h"
#include "Object/EquipmentInstance.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/StaticDataSubsystem.h"

void USK_GA_ConsumableItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CheckCooldown(Handle, ActorInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	USKInventoryItemData* ItemData = GetItemDataFromItemID();
	if (!ItemData)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	USKConsumableItemData* ActiveItemData = Cast<USKConsumableItemData>(ItemData);
	if (!ActiveItemData)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UGameplayEffect* EffectInstance = GetCooldownGameplayEffect();
	if (!EffectInstance)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	TSubclassOf<UGameplayEffect> EffectClass = EffectInstance->GetClass();
	FGameplayEffectSpecHandle CooldownSpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, GetAbilityLevel());

	if (CooldownSpecHandle.IsValid())
	{
		if (ActiveItemData->Cooldown >= 0.1f)
		{
			CooldownSpecHandle.Data->SetSetByCallerMagnitude(TAG_Item_Cooldown, ActiveItemData->Cooldown);
		}
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	ASC->ApplyGameplayEffectSpecToSelf(*CooldownSpecHandle.Data);
	
	UAnimMontage* MontageToPlay = ActiveItemData->UseMontage;
	
	if (!MontageToPlay)
	{	
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
	UAbilityTask_PlayMontageAndWait* PlayAnimTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("UseItem"), MontageToPlay);

	PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
	PlayAnimTask->OnCancelled.AddDynamic(this, &ThisClass::OnCanceled);
	
	SetWeaponVisible(false);
	PlayAnimTask->ReadyForActivation();
}

void USK_GA_ConsumableItem::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	SetWeaponVisible(true);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

USKInventoryItemData* USK_GA_ConsumableItem::GetItemDataFromItemID() const
{
	UGameInstance* GI = GetWorld()->GetGameInstance();
	if (!GI)
	{
		return nullptr;
	}

	UStaticDataSubsystem* SDSubSystem = GI->GetSubsystem<UStaticDataSubsystem>();
	if (!SDSubSystem)
	{
		return nullptr;
	}

	const FItemData* ItemData = SDSubSystem->GetData<FItemData>(ItemID);
	if (!ItemData)
	{
		return nullptr;
	}

	return ItemData->InventoryItemDataAsset.Get();
}

void USK_GA_ConsumableItem::OnCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("USK_GA_ConsumableItem::OnCompleted"));
	USKInventoryItemData* ItemData = GetItemDataFromItemID();
	if (!ItemData)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	USKConsumableItemData* ActiveItemData = Cast<USKConsumableItemData>(ItemData);
	if (!ActiveItemData)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ActiveItemData->ConsumableGE, GetAbilityLevel(), ASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	for (const FUseItemData& Param : ActiveItemData->Params)
	{
		if (!Param.DataTag.IsValid()) continue;

		SpecHandle.Data->SetSetByCallerMagnitude(
			Param.DataTag,
			Param.Magnitude
		);
	}

	UGameplayEffect* EffectInstance = GetCooldownGameplayEffect();
	if (!EffectInstance)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	TSubclassOf<UGameplayEffect> EffectClass = EffectInstance->GetClass();
	FGameplayEffectSpecHandle CooldownSpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, GetAbilityLevel());

	if (CooldownSpecHandle.IsValid())
	{
		if (ActiveItemData->Cooldown >= 0.1f)
		{
			CooldownSpecHandle.Data->SetSetByCallerMagnitude(TAG_Item_Cooldown, ActiveItemData->Cooldown);
		}
	}

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void USK_GA_ConsumableItem::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USK_GA_ConsumableItem::SetWeaponVisible(bool bVisible)
{
	UE_LOG(LogTemp, Warning, TEXT("[SetWeaponVisible] 호출됨. bVisible = %s"), bVisible ? TEXT("true") : TEXT("false"));

	if (!CurrentActorInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("[SetWeaponVisible] CurrentActorInfo == nullptr"));
		return;
	}

	if (!CurrentActorInfo->PlayerController.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[SetWeaponVisible] PlayerController == nullptr"));
		return;
	}

	APlayerState* PS = CurrentActorInfo->PlayerController->PlayerState;

	if (!PS)
	{
		UE_LOG(LogTemp, Error, TEXT("[SetWeaponVisible] PlayerState == nullptr"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[SetWeaponVisible] PlayerState: %s"), *PS->GetName());

	ASKPlayerState* SKPS = Cast<ASKPlayerState>(PS);
	if (!SKPS)
	{
		UE_LOG(LogTemp, Error, TEXT("[SetWeaponVisible] PlayerState 캐스팅 실패 (ASKPlayerState 아님)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[SetWeaponVisible] ASKPlayerState 캐스팅 성공"));

	UEquipmentComponent* EquipComp = PS->FindComponentByClass<UEquipmentComponent>();
	if (!EquipComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[SetWeaponVisible] EquipmentComponent 찾기 실패"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[SetWeaponVisible] EquipmentComponent 찾기 성공"));

	// 장비 가져오기
	const FEquipmentSlotData* WeaponSlot = EquipComp->GetEquipment(EEquipmentSlotType::Weapon);
	
	if (!WeaponSlot)
	{
		UE_LOG(LogTemp, Error, TEXT("[SetWeaponVisible] Weapon 슬롯을 찾을 수 없음"));
		return;
	}

	if (!WeaponSlot->EquipmentInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[SetWeaponVisible] WeaponInstance == nullptr (장비 없음)"));
		return;
	}

	UEquipmentInstance* WeaponInstance = WeaponSlot->EquipmentInstance;

	UE_LOG(LogTemp, Warning, TEXT("[SetWeaponVisible] WeaponInstance 가져오기 성공: %s"), *WeaponInstance->GetName());

	// 시각적 토글
	WeaponInstance->SpawnActorVisible(bVisible);
	UE_LOG(LogTemp, Warning, TEXT("[SetWeaponVisible] SpawnActorVisible 호출 완료"));
}