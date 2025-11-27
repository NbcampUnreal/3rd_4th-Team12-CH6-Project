#include "Character/AI/SKAICharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"
#include "SKAIDataAsset.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utility/StaticDataSubsystem.h"
#include "GameData/StaticData/MonsterDataTable.h"

ASKAICharacterBase::ASKAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // or Full

	AttributeSet = CreateDefaultSubobject<USKAIAttributeSet>(TEXT("AttributeSet"));
}

void ASKAICharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	StartLocation = GetActorLocation();
}

void ASKAICharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASKAICharacterBase, AIDataAsset, COND_InitialOnly);
}

UAbilitySystemComponent* ASKAICharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASKAICharacterBase::InitializeAttributeSetAndAbilitiesFromDataAsset()
{

	UE_LOG(LogTemp, Warning, TEXT("Monster Setting"));
	
	if (!AIDataAsset.Get())
	{
		AIDataAsset.LoadSynchronous();
	}

	if (!HasAuthority() || !AIDataAsset.Get())
	{
		return;
	}

	//정적데이터 적용
	ApplyStaticMonsterStats();

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : AIDataAsset->StartupAbilities)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0, this));
		}
	}

	FGameplayEffectContextHandle GEContextHandle = AbilitySystemComponent->MakeEffectContext();
	for (const TSubclassOf<UGameplayEffect>& GameEffectClass : AIDataAsset->StartupGE)
	{
		if (GameEffectClass)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(GameEffectClass->GetDefaultObject<UGameplayEffect>(), 1.f, GEContextHandle);
		}
	}
	
	if (AIDataAsset->TeamTag.IsValid())
	{
		AbilitySystemComponent->AddLooseGameplayTag(AIDataAsset->TeamTag);
	}
		
	if (AIDataAsset->GiveTeamTagEffect)
	{
		FGameplayEffectContextHandle GEContextHandle_ = AbilitySystemComponent->MakeEffectContext();
		GEContextHandle_.AddSourceObject(this);

		FGameplayEffectSpecHandle GESpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			AIDataAsset->GiveTeamTagEffect,
			1.0f,
			GEContextHandle_
		);

		if (GESpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*GESpecHandle.Data.Get());
		}
	}
}

TArray<UAnimMontage*> ASKAICharacterBase::GetMontages() const
{
	return Montages;
}

FVector ASKAICharacterBase::GetStartLocation() const
{
	return StartLocation;
}

void ASKAICharacterBase::ApplyStaticMonsterStats()
{
	if (MonsterID < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("MonsterID not set for %s"), *GetName());
		return;
	}

	UStaticDataSubsystem* SDS = GetGameInstance()->GetSubsystem<UStaticDataSubsystem>();
	if (!SDS)
	{
		UE_LOG(LogTemp, Error, TEXT("StaticDataSubsystem Missing!"));
		return;
	}

	const FMonsterData* Data = SDS->GetData<FMonsterData>(MonsterID);
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("Monster StaticData Not Found: ID = %d"), MonsterID);
		return;
	}

	// ---- 실제 스탯 적용 (AttributeSet or 내부 변수) ----
	AttributeSet->SetHealth(Data->MaxHealth);
	AttributeSet->SetMaxHealth(Data->MaxHealth);
	AttributeSet->SetAttack(Data->Attack);
	AttributeSet->SetArmor(Data->Armor);
	AttributeSet->SetPoise(Data->Poise);
	AttributeSet->SetSpeed(Data->Speed);


	// 예시: 이동 속도 적용
	GetCharacterMovement()->MaxWalkSpeed = Data->Speed;

	// DropTableID 설정 필요 시 저장
	DropTableID = Data->DropTableID;

	UE_LOG(LogTemp, Log, TEXT("[AI StaticData] %s : (HP=%f, Atk=%f, Def=%f)"),
		*GetName(), Data->MaxHealth, Data->Attack, Data->Armor);
}
