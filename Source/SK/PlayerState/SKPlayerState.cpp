// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/SKPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameData/WeaponDataRow.h"
#include "Weapon/SKWeaponData.h"
#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/QuickSlotComponent.h"
#include "Component/SKCombatComponent.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Utility/SKUIManagerSubSystem.h"
#include "Utility/StaticDataSubsystem.h"
#include "GameData/StaticData/LevelUpData.h"

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
	{
		OnRep_CurrentWeaponTag();
		SetDAPlayerStat();
	}

	//다른 방법 있으면 추후 변경 예정 현재는 기능 테스트 용으로 추가
	APlayerController* PC = GetPlayerController();
	if (!PC)
		return;
	
	USKUIManagerSubSystem* UISubSystem = ULocalPlayer::GetSubsystem<USKUIManagerSubSystem>(PC->GetLocalPlayer());
	if (!UISubSystem) return;

	UISubSystem->SettingLayout();

	if (AbilitySystemComponent)
	{
		// ASC Delegate 바인딩
		AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &ASKPlayerState::HandleGameplayEffectAdded);
		AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &ASKPlayerState::HandleGameplayEffectRemoved);
	}

	SDS = GetGameInstance()->GetSubsystem<UStaticDataSubsystem>();
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
	
	if (InventoryComponent && NewPS->InventoryComponent)
	{
		InventoryComponent->CopyTo(NewPS->InventoryComponent);
	}
	
	if (EquipmentComponent && NewPS->EquipmentComponent)
	{
		EquipmentComponent->CopyTo(NewPS->EquipmentComponent);
	}
	
	if (QuickSlotComponent && NewPS->QuickSlotComponent)
	{
		QuickSlotComponent->CopyTo(NewPS->QuickSlotComponent);
	}

	NewPS->Gold = Gold;
	NewPS->OldGold = OldGold;
	NewPS->Level = Level;
	NewPS->AbilityPoint = AbilityPoint;
}

void ASKPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASKPlayerState, CharacterData, COND_InitialOnly);
	DOREPLIFETIME(ASKPlayerState, AbilitySystemComponent);  
	DOREPLIFETIME(ASKPlayerState, CurrentWeaponTag);
	DOREPLIFETIME(ASKPlayerState, Gold);
	DOREPLIFETIME(ASKPlayerState, OldGold);
	DOREPLIFETIME(ASKPlayerState, Level);
	DOREPLIFETIME(ASKPlayerState, AbilityPoint);
}

void ASKPlayerState::SetTeamFromTag(const FGameplayTag& TeamTag)
{
	if (TeamTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Team.Player")))
	{
		PlayerTeamID = FGenericTeamId(0);
	}
	else if (TeamTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Team.Monster")))
	{
		PlayerTeamID = FGenericTeamId(1);
	}
	else
	{
		PlayerTeamID = FGenericTeamId::NoTeam; // 255 Neutral
	}

	UE_LOG(LogTemp, Log, TEXT("Player TeamID Set: %d"), PlayerTeamID.GetId());
}


void ASKPlayerState::SetCurWeaponTag(FGameplayTag NewTag)
{
	CurrentWeaponTag = NewTag;
	ASKPlayerCharacter* PC = GetPawn<ASKPlayerCharacter>();
	if (PC)
	{
		PC->SetTraceSocket();  
	}
}

void ASKPlayerState::EquipmentComponentSetting()
{
	if (HasAuthority())
	{
		EquipmentComponent->ReSpawnWeapon();
	}
}

void ASKPlayerState::OnRep_CurrentWeaponTag()
{
	ASKPlayerCharacter* PC = GetPawn<ASKPlayerCharacter>();
	if (PC)
	{
		PC->SetTraceSocket();  
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

	if (WeaponSocketDT.IsNull())
		return EmptyResult;

	// CurrentWeaponTag == RowName 으로 가정
	FName RowName = CurrentWeaponTag.GetTagName();

	const FSKWeaponDataRow* Row = WeaponSocketDT->FindRow<FSKWeaponDataRow>(RowName, TEXT("GetTraceSockets"));
	if (!Row)
		return EmptyResult;

	return Row->TraceSockets;
}

TSoftObjectPtr<UDataTable> ASKPlayerState::GetWeaponDT() const
{
	return WeaponSocketDT;
}

TSoftObjectPtr<UDataTable> ASKPlayerState::GetWeaponData() const
{
	return WeaponDataTable;
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

			SetTeamFromTag(CharacterData->TeamTag);
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

const FSKWeaponDataRow* ASKPlayerState::GetWeaponSocketDataRow() const
{
	UDataTable* DT = WeaponSocketDT.LoadSynchronous();
	if (!DT)
		return nullptr;

	FString TagName = CurrentWeaponTag.GetTagName().ToString();
	return DT->FindRow<FSKWeaponDataRow>(FName(*TagName), TEXT(""));
}

const FWeaponDataRow* ASKPlayerState::GetWeaponDataRow() const
{
	UDataTable* DT = WeaponDataTable.LoadSynchronous();
	if (!DT)
		return nullptr;

	FString FullTag = CurrentWeaponTag.GetTagName().ToString();
	FString RowString;

	// 마지막 . 뒤의 문자열만 추출
	FullTag.Split(TEXT("."), nullptr, &RowString, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	FName RowName = FName(*RowString);
	return DT->FindRow<FWeaponDataRow>(RowName, TEXT("GetWeaponDataRow"));
}

FWeaponDataRow& ASKPlayerState::GetWeaponData()
{
	static FWeaponDataRow DefaultRow; 

	if (!WeaponDT) return DefaultRow;

	FString FullTag = CurrentWeaponTag.GetTagName().ToString();
	FString RowString;

	// 마지막 . 뒤의 문자열만 추출
	FullTag.Split(TEXT("."), nullptr, &RowString, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	FName RowName = FName(*RowString);
		return *WeaponDT->FindRow<FWeaponDataRow>(RowName,TEXT("GetWeaponDataRow"));
}


FGameplayTag ASKPlayerState::GetWeaponTag() const
{
	return CurrentWeaponTag;
}

void ASKPlayerState::HandleGameplayEffectAdded(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec,
	FActiveGameplayEffectHandle Handle)
{
	FGameplayTagContainer BuffTags = Spec.Def->GetGrantedTags();

	float Duration = -1.f;
	if (const FActiveGameplayEffect* ActiveGE = ASC->GetActiveGameplayEffect(Handle))
	{
		Duration = ActiveGE->GetDuration();
	}

	if (Duration <= -1.f)
	{
		return;
	}
	
	TArray<FModifiedAttributeInfo> ModifiedAttributes;
	int32 TempIndex = 0;
	for (const FGameplayModifierInfo& Mod : Spec.Def->Modifiers)
	{
		FModifiedAttributeInfo Info;
		Info.Attribute = Mod.Attribute;
		Info.Op = Mod.ModifierOp;
		Info.Magnitude = Spec.GetModifierMagnitude(TempIndex++);
		Info.Duration = Duration;
		ModifiedAttributes.Add(Info);
	}

	FModifiedAttributeArray ModifiedArray;
	ModifiedArray.Items = ModifiedAttributes;
	ModifiedAttributeMap.Add(Handle, ModifiedArray);
	
	OnBuffAdded.Broadcast(Handle, ModifiedArray, BuffTags, Duration);
	
	if (FActiveGameplayEffectEvents* Events = ASC->GetActiveEffectEventSet(Handle))
	{
		Events->OnStackChanged.AddUObject(this, &ASKPlayerState::HandleGameplayEffectStackChange);
		Events->OnTimeChanged.AddUObject(this, &ASKPlayerState::HandleGameplayEffectTimeChange);
	}
	UE_LOG(LogTemp, Log, TEXT("[BuffAdd] Duration: %.2f"),Duration);
}

void ASKPlayerState::HandleGameplayEffectRemoved(const FActiveGameplayEffect& Effect)
{
	if (FModifiedAttributeArray* FoundArray = ModifiedAttributeMap.Find(Effect.Handle))
	{
		FModifiedAttributeArray RemovedModified = *FoundArray;
		
		OnBuffRemoved.Broadcast(Effect.Handle, RemovedModified);
		ModifiedAttributeMap.Remove(Effect.Handle);
	}
}

void ASKPlayerState::HandleGameplayEffectStackChange(FActiveGameplayEffectHandle Handle, int32 NewStack, int32 OldStack)
{
	if (FModifiedAttributeArray* FoundArray = ModifiedAttributeMap.Find(Handle))
	{
		OnBuffStackChanged.Broadcast(Handle, NewStack, OldStack);
	}
}

void ASKPlayerState::HandleGameplayEffectTimeChange(FActiveGameplayEffectHandle Handle, float NewStartTime, float NewDuration)
{
	if (FModifiedAttributeArray* FoundArray = ModifiedAttributeMap.Find(Handle))
	{
		float Duration = 0.0f;
		if (FoundArray->Items.Num() > 0)
		{
			Duration = FoundArray->Items[0].Duration;
			
		}

		OnBuffTimeChanged.Broadcast(Handle, NewStartTime, Duration);
	}
}

void ASKPlayerState::AddGold(int32 Value)
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Error, TEXT("[PlayerState] AddGold %d,   (%d + %d = %d)"), Value, Gold, Value, Gold+Value);
	OldGold = Gold;
	Gold += Value;
	OnRep_Gold();
}

int32 ASKPlayerState::GetRequiredGoldForNextLevel() const
{
	if (!SDS) return -1;
	const FLevelUpData* Rule = SDS->GetData<FLevelUpData>(Level);
	return Rule ? Rule->RequiredGold : -1;
}

void ASKPlayerState::OnRep_Gold()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;
	
	if (PC->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayerState] OnRep_Gold  누적 Gold : %d"),Gold);
		GoldChanged.Broadcast(Gold, OldGold);
	}
	
}
void ASKPlayerState::OnRep_Level()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;
	
	if (PC->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayerState] OnRep_Level Level : %d"),Level);
	}
}
void ASKPlayerState::OnRep_AbilityPoint() {}

void ASKPlayerState::Server_RequestLevelUp_Implementation()
{
	TryLevelUp();
}

void ASKPlayerState::ConsumeAbilityPoint()
{
	if (AbilityPoint > 0)
	{
		AbilityPoint--;
		OnRep_AbilityPoint();
	}
}

void ASKPlayerState::TryLevelUp()
{
	//if (!HasAuthority() || !SDS)
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[LevelUp] !HasAuthority()"));
		return;
	}

	if (!SDS)
	{
		SDS = GetGameInstance()->GetSubsystem<UStaticDataSubsystem>();
	}

	const FLevelUpData* Rule = SDS->GetData<FLevelUpData>(Level);
	if (!Rule)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LevelUp] No level data found for Level %d"), Level);
		return;
	}

	// Gold 부족
	if (Gold < Rule->RequiredGold)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LevelUp] Gold 부족! 필요:%d, 현재:%d"), Rule->RequiredGold, Gold);
		return;
	}

	// 골드 지불
	Gold -= Rule->RequiredGold;
	OnRep_Gold();

	// 레벨 증가
	Level++;
	OnRep_Level();

	// AbilityPoint 지급
	AbilityPoint += Rule->AbilityPointReward;
	OnRep_AbilityPoint();

	UE_LOG(LogTemp, Log, TEXT("[LevelUp] 성공! New Level=%d, AbilityPoint=%d"), Level, AbilityPoint);
}