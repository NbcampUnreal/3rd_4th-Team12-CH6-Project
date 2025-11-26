// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Item/SK_GA_ConsumableItem.h"

#include "AbilitySystemComponent.h"
#include "GameData/StaticData/ItemDataTable.h"
#include "Item/Inventory/Data/SKConsumableItemData.h"
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
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	USKConsumableItemData* ActiveItemData = Cast<USKConsumableItemData>(ItemData);
	if (!ActiveItemData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ActiveItemData->ConsumableGE, GetAbilityLevel(), ASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
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
		return;
	}
	TSubclassOf<UGameplayEffect> EffectClass = EffectInstance->GetClass();
	FGameplayEffectSpecHandle CooldownSpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, GetAbilityLevel());

	if (CooldownSpecHandle.IsValid())
	{
		CooldownSpecHandle.Data->SetSetByCallerMagnitude(TAG_Item_Consume_Data_Cooldown, ActiveItemData->Cooldown);
	}
	
	ASC->ApplyGameplayEffectSpecToSelf(*CooldownSpecHandle.Data);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void USK_GA_ConsumableItem::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
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
