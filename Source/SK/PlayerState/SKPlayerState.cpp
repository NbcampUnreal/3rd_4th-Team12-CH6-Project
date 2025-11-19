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

UAbilitySystemComponent* ASKPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USKAttributeSet* ASKPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}


void ASKPlayerState::SetDAPlayerStat()
{
	if (!CharacterData.Get())
		CharacterData.LoadSynchronous();

	if (!HasAuthority() || !CharacterData.Get())
		return;
	// AttributeSet의 초기값을 데이터 에셋의 값으로 설정
	AttributeSet->SetSpeed(CharacterData->Speed);
	AttributeSet->SetSprintWeight(CharacterData->SprintWeight);
	AttributeSet->SetHealth(CharacterData->Health);
	AttributeSet->SetMaxHealth(CharacterData->MaxHealth);
	AttributeSet->SetStamina(CharacterData->Stamina);
	AttributeSet->SetMaxStamina(CharacterData->MaxStamina);
	AttributeSet->SetHeat(CharacterData->Heat);
	AttributeSet->SetMaxHeat(CharacterData->MaxHeat);
	AttributeSet->SetExp(CharacterData->Exp);
	AttributeSet->SetLevel(CharacterData->Level);
	AttributeSet->SetGold(CharacterData->Gold);
	AttributeSet->SetAttack(CharacterData->Attack);
	AttributeSet->SetArmor(CharacterData->Armor);
	AttributeSet->SetPoise(CharacterData->Poise);

	// if (GetCharacterMovement())
	// {
	// 	GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetSpeed();
	//
	// 	// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	// 	// 	USKAttributeSet::GetSpeedAttribute()).AddUObject(this, &ASKCharacterBase::OnSpeedAttributeChanged);
	// }
	//이 부분을 캐릭터에서

	//JobDataAsset - Give Ability
	int32 InputID = 0;
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterData->StartupAbilities)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, InputID, this));
			InputID++;
		}
	}

	FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
	//JobDataAsset - Give GE
	for (const TSubclassOf<UGameplayEffect>& GameEffectClass : CharacterData->StartupGE)
	{
		if (GameEffectClass)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(GameEffectClass->GetDefaultObject<UGameplayEffect>(), 1.f, Ctx);
		}
	}


	if (CharacterData->TeamTag.IsValid())
	{
		AbilitySystemComponent->AddLooseGameplayTag(CharacterData->TeamTag);
	}
		
	if (CharacterData->GiveTeamtagEffect)
	{
		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			CharacterData->GiveTeamtagEffect,
			1.0f,
			ContextHandle
		);

		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}