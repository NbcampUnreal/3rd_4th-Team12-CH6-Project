// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/SKPlayerState.h"

#include "AbilitySystemComponent.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/QuickSlotComponent.h"

ASKPlayerState::ASKPlayerState()
{
	// ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // or Full

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<USKAttributeSet>(TEXT("AttributeSet"));

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

void ASKPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASKPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
