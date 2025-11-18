// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/SKPlayerState.h"

#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/QuickSlotComponent.h"

ASKPlayerState::ASKPlayerState()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	if (InventoryComponent)
	{
		InventoryComponent->SetIsReplicated(true);
	}
 
	QuickSlotComponent = CreateDefaultSubobject<UQuickSlotComponent>(TEXT("QuickSlotComponent"));
	if (QuickSlotComponent)
	{
		QuickSlotComponent->SetIsReplicated(true);
	}
 
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	if (EquipmentComponent)
	{
		EquipmentComponent->SetIsReplicated(true);
	}
}

void ASKPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	ASKPlayerState* NewPS = Cast<ASKPlayerState>(NewPlayerState);
	if (!NewPS) return;
	
	//데이터 복사 예시
	//NewPS->A = A; 
	
}

void ASKPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
