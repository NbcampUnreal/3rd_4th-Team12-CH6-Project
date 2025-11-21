// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/SKPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameData/WeaponDataRow.h"
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

	if (AbilitySystemComponent)
	{
		OnASCReady.Broadcast();
	}
	if (HasAuthority())
		OnRep_CurrentWeaponTag();
}

void ASKPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASKPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	ASKPlayerState* NewPS = Cast<ASKPlayerState>(NewPlayerState);
	if (!NewPS)
		return;
	//데이터 복사 예시
	//NewPS->A = A; 
}

void ASKPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASKPlayerState, CharacterData, COND_InitialOnly);
}


void ASKPlayerState::OnRep_CurrentWeaponTag()
{
	ASKPlayerCharacter* PC = GetPawn<ASKPlayerCharacter>();
	if (PC)
	{
		PC->SetTraceSocket();   // 여기서 호출해야 안전!!
	}
}


UAbilitySystemComponent* ASKPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USKAttributeSet* ASKPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

TArray<FName> ASKPlayerState::GetTraceSocket()
{
	TArray<FName> EmptyResult;

	if (!IsValid(CurrentWeaponDT))
		return EmptyResult;

	// CurrentWeaponTag == RowName 으로 가정
	FName RowName = CurrentWeaponTag.GetTagName();

	const FSKWeaponDataRow* Row = CurrentWeaponDT->FindRow<FSKWeaponDataRow>(RowName, TEXT("GetTraceSockets"));
	if (!Row)
		return EmptyResult;

	return Row->TraceSockets;
}

const UDataTable* ASKPlayerState::GetWeaponDT() const
{
	return CurrentWeaponDT;
}

int32 ASKPlayerState::GetMaxComobo(bool bLeft)
{
	if (!CurrentWeaponDT)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponDataTable is null!"));
		return 1;
	}

	// CurrentWeaponTag를 RowName(=TagName)으로 사용
	FName RowName = CurrentWeaponTag.GetTagName();

	if (RowName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentWeaponTag is None!"));
		return 1;
	}

	// DataTable에서 Row 찾기
	const FSKWeaponDataRow* Row = CurrentWeaponDT->FindRow<FSKWeaponDataRow>(
		RowName,
		TEXT("GetMaxCombo")
	);

	if (!Row)
	{
		return 1;
	}

	// 좌/우에 따라 MaxCombo 반환
	return bLeft ? Row->MaxLeftCombo : Row->MaxRightCombo;
}


void ASKPlayerState::SetDAPlayerStat()
{
	if (CharacterData.IsNull())
		return;

	CharacterData.LoadSynchronous();
	if (!CharacterData.Get())
		return;

	if (HasAuthority())
	{
		// Attribute 초기값 서버가 세팅
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

		// Ability 부여
		int32 InputID = 0;
		for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterData->StartupAbilities)
		{
			if (AbilityClass)
			{
				AbilitySystemComponent->GiveAbility(
					FGameplayAbilitySpec(AbilityClass, 1, InputID, this));
				InputID++;
			}
		}

		// GE 적용
		FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
		for (const TSubclassOf<UGameplayEffect>& GEClass : CharacterData->StartupGE)
		{
			if (GEClass)
			{
				AbilitySystemComponent->ApplyGameplayEffectToSelf(
					GEClass->GetDefaultObject<UGameplayEffect>(),
					1.f,
					Ctx
				);
			}
		}
		// 태그 부여
		if (CharacterData->TeamTag.IsValid())
		{
			AbilitySystemComponent->AddLooseGameplayTag(CharacterData->TeamTag);
		}

		// 태그 GE 적용
		if (CharacterData->GiveTeamtagEffect)
		{
			FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
				CharacterData->GiveTeamtagEffect, 1.0f, ContextHandle);

			if (SpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void ASKPlayerState::SetWeaponTag(FGameplayTag WeaponTag)
{
	CurrentWeaponTag = WeaponTag;
}

FGameplayTag ASKPlayerState::GetWeapontTag() const
{
	return CurrentWeaponTag;
}
