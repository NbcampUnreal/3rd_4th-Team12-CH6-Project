// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/EquipmentInstance.h"

#include "Net/UnrealNetwork.h"

void UEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentInstance, GrantedAbilityHandle);
	DOREPLIFETIME(UEquipmentInstance, GrantedEffectHandle);
}
