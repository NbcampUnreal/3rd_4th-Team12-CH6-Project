#include "Character/AI/SKAICharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"
#include "SKAIDataAsset.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utility/StaticDataSubsystem.h"
#include "GameData/StaticData/MonsterDataTable.h"
#include "Perception/AISense_Damage.h"

ASKAICharacterBase::ASKAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;

	GetCapsuleComponent()->SetCollisionProfileName("AI");
	
	CombatArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp|CombatArea"));
	CombatArea->SetupAttachment(GetRootComponent());
	CombatArea->SetCollisionProfileName("CombatArea");
	CombatArea->OnComponentBeginOverlap.AddDynamic(this, &ASKAICharacterBase::OnCombatAreaBeginOverlap);
	CombatArea->OnComponentEndOverlap.AddDynamic(this, &ASKAICharacterBase::OnCombatAreaEndOverlap);

	MeleeOrRushArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp|MeleeOrRushArea"));
	MeleeOrRushArea->SetupAttachment(GetRootComponent());
	MeleeOrRushArea->SetCollisionProfileName("CombatArea");
	MeleeOrRushArea->OnComponentBeginOverlap.AddDynamic(this, &ASKAICharacterBase::OnMeleeOrRushAreaBeginOverlap);
	MeleeOrRushArea->OnComponentEndOverlap.AddDynamic(this, &ASKAICharacterBase::OnMeleeOrRushAreaEndOverlap);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // or Full
	
	AttributeSet = CreateDefaultSubobject<USKAIAttributeSet>(TEXT("AttributeSet"));
}

void ASKAICharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(USKAIAttributeSet::GetHealthAttribute()).AddUObject(this, &ASKAICharacterBase::OnHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(USKAIAttributeSet::GetStaminaAttribute()).AddUObject(this, &ASKAICharacterBase::OnStaminaChanged);
}

void ASKAICharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}
	
	float Damage = Data.OldValue - Data.NewValue;

	FName PlayerAttackType;
	
	AActor* VictimActor = this;
	AActor* InstigatorActor = nullptr;
	
	if (const FGameplayEffectModCallbackData* ModData = Data.GEModData)
	{
		const FGameplayEffectSpec& EffectSpec = ModData->EffectSpec;
		if (EffectSpec.DynamicGrantedTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Attack.Normal"))))
		{
			PlayerAttackType = "Normal";
		}
		else if (EffectSpec.DynamicGrantedTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Attack.Heavy"))))
		{
			PlayerAttackType = "Heavy";
		}
		else if (EffectSpec.DynamicGrantedTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Attack.UnGuardable"))))
		{
			PlayerAttackType = "UnGuardable";
		}
		
		const FGameplayEffectContextHandle& EffectContextHandle = ModData->EffectSpec.GetEffectContext();
		if (const FGameplayEffectContext* EffectContext = EffectContextHandle.Get())
		{
			InstigatorActor = EffectContext->GetInstigator();
		}
	}

	if (!FMath::IsNearlyZero(Data.NewValue))
	{
		if (Damage > 0.f && IsValid(VictimActor) && IsValid(InstigatorActor))
		{
			if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Boss"))))
			{
				if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction"))))
				{
					AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction")));
			
					AbilitySystemComponent->CancelAllAbilities();
				}
			}
			else
			{
				if (PlayerAttackType == "UnGuardable")
				{
					if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction"))))
					{
						AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction")));
			
						AbilitySystemComponent->CancelAllAbilities();
					}
				}
			}
		
			UAISense_Damage::ReportDamageEvent(
			VictimActor,
			VictimActor,        
			InstigatorActor,   
			Damage,       
			VictimActor->GetActorLocation(),            
			VictimActor->GetActorLocation()
			);
		}
	}
	else
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Death"))))
		{
			AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Death")));
			
			AbilitySystemComponent->CancelAllAbilities();
		}
	}
}

void ASKAICharacterBase::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	float CurrentStamina = Data.NewValue;

	if (CurrentStamina > 0.f)
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Blocked"))))
		{
			AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Blocked")));

			AbilitySystemComponent->CancelAllAbilities();
		}
	}
	else if (FMath::IsNearlyZero(CurrentStamina))
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Groggy"))))
		{
			AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Groggy")));

			AbilitySystemComponent->CancelAllAbilities();
		}
	}
}

void ASKAICharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	StartLocation = GetActorLocation();
}

void ASKAICharacterBase::OnCombatAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	AddTag(FGameplayTag::RequestGameplayTag("AI.Combat"));
	
	SendEventToASC(nullptr, nullptr, FGameplayTag::RequestGameplayTag("Event.EndAbility"));
}

void ASKAICharacterBase::OnCombatAreaEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
	)
{
	RemoveTag(FGameplayTag::RequestGameplayTag("AI.Combat"));
	
	SendEventToASC(nullptr, nullptr, FGameplayTag::RequestGameplayTag("Event.EndAbility"));
}

void ASKAICharacterBase::OnMeleeOrRushAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	AddTag(FGameplayTag::RequestGameplayTag("AI.Melee"));
	
	SendEventToASC(nullptr, nullptr, FGameplayTag::RequestGameplayTag("Event.EndAbility"));
}

void ASKAICharacterBase::OnMeleeOrRushAreaEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
	)
{
	RemoveTag(FGameplayTag::RequestGameplayTag("AI.Melee"));
	
	SendEventToASC(nullptr, nullptr, FGameplayTag::RequestGameplayTag("Event.EndAbility"));
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

	/*
	FGameplayEffectContextHandle GEContextHandle = AbilitySystemComponent->MakeEffectContext();
	for (const TSubclassOf<UGameplayEffect>& GameEffectClass : AIDataAsset->StartupGE)
	{
		if (GameEffectClass)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(GameEffectClass->GetDefaultObject<UGameplayEffect>(), 1.f, GEContextHandle);
		}
	}
	*/
	
	if (AIDataAsset->TeamTag.IsValid())
	{
		AddTag(AIDataAsset->TeamTag);
	}

	if (AIDataAsset->TypeTag.IsValid())
	{
		AddTag(AIDataAsset->TypeTag);
	}
	/*
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
	*/
	if (!AIDataAsset->Montages.IsEmpty())
	{
		Montages = AIDataAsset->Montages;
	}

	if (!AIDataAsset->ProjectileClasses.IsEmpty())
	{
		ProjectileClasses = AIDataAsset->ProjectileClasses;
	}
	
	if (AIDataAsset->StateTreeAsset)
	{
		StateTreeAsset = AIDataAsset->StateTreeAsset;
	}
}

void ASKAICharacterBase::AddTag(FGameplayTag Tag) const
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}
	
	AbilitySystemComponent->AddLooseGameplayTag(Tag);
}

void ASKAICharacterBase::RemoveTag(FGameplayTag Tag) const
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}
	
	AbilitySystemComponent->RemoveLooseGameplayTag(Tag);
}

void ASKAICharacterBase::SendEventToASC(AActor* LocalInstigator, AActor* LocalTargetActor, FGameplayTag EventTag) const
{
	FGameplayEventData EventData;
	EventData.Instigator = LocalInstigator;
	EventData.Target = LocalTargetActor;
	EventData.EventTag = EventTag;
	EventData.OptionalObject = nullptr;

	AbilitySystemComponent->HandleGameplayEvent(EventData.EventTag, &EventData);
}

FName ASKAICharacterBase::GetMonsterName() const
{
	return MonsterName;
}

TMap<FName, TObjectPtr<UAnimMontage>> ASKAICharacterBase::GetMontages() const
{
	return Montages;
}

TMap<FName, TSubclassOf<ASKBaseProjectile>> ASKAICharacterBase::GetProjectileClasses() const
{
	return ProjectileClasses;
}

TObjectPtr<UStateTree> ASKAICharacterBase::GetStateTreeAsset() const
{
	return StateTreeAsset;
}

int32 ASKAICharacterBase::GetMaxMeleeIndex() const
{
	return MaxMeleeIndex;
}

int32 ASKAICharacterBase::GetMaxRushIndex() const
{
	return MaxRushIndex;
}

int32 ASKAICharacterBase::GetMaxJumpRushIndex() const
{
	return MaxJumpRushIndex;
}

int32 ASKAICharacterBase::GetMaxFlyRushIndex() const
{
	return MaxFlyRushIndex;
}

FVector ASKAICharacterBase::GetStartLocation() const
{
	return StartLocation;
}

float ASKAICharacterBase::GetBackstepDistance() const
{
	return BackstepDistance;
}

int32 ASKAICharacterBase::GetDropTableID() const
{
	return DropTableID;
}

FMonsterData ASKAICharacterBase::GetMonsterData() const
{
	return *MonsterData;
}

void ASKAICharacterBase::SetOverlayMaterial(UMaterialInterface* OverlayMat, float Duration)
{
	if (!GetMesh() || !OverlayMat)
		return;

	GetMesh()->SetOverlayMaterial(OverlayMat);

	// Duration 시간이 지나면 제거
	if (Duration > 0.f)
	{
	
		GetWorld()->GetTimerManager().SetTimer(
			OverlayTimerHandle,
			this,
			&ASKAICharacterBase::ClearOverlayMaterial,
			Duration,
			false
		);
	} 
}

void ASKAICharacterBase::ClearOverlayMaterial()
{
	if (!GetMesh())
		return;
	UE_LOG(LogTemp, Error, TEXT("[Cam] Clear Material"));
	GetMesh()->SetOverlayMaterial(nullptr);
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

	MonsterData = SDS->GetData<FMonsterData>(MonsterID);
	if (!MonsterData)
	{
		UE_LOG(LogTemp, Error, TEXT("Monster StaticData Not Found: ID = %d"), MonsterID);
		return;
	}

	// ---- 실제 스탯 적용 (AttributeSet or 내부 변수) ----
	AttributeSet->SetHealth(MonsterData->MaxHealth);
	AttributeSet->SetMaxHealth(MonsterData->MaxHealth);
	AttributeSet->SetStamina(MonsterData->MaxStamina);
	AttributeSet->SetMaxStamina(MonsterData->MaxStamina);
	AttributeSet->SetAttack(MonsterData->Attack);
	AttributeSet->SetArmor(MonsterData->Armor);
	AttributeSet->SetPoise(MonsterData->Poise);
	AttributeSet->SetSpeed(MonsterData->Speed);
	MonsterName = MonsterData->MonsterName;
	MaxMeleeIndex = MonsterData->MaxMeleeIndex;
	MaxRushIndex = MonsterData->MaxRushIndex;
	MaxJumpRushIndex = MonsterData->MaxJumpRushIndex;
	MaxFlyRushIndex = MonsterData->MaxFlyRushIndex;
	BackstepDistance = MonsterData->BackstepDistance;
	
	// 예시: 이동 속도 적용
	GetCharacterMovement()->MaxWalkSpeed = MonsterData->Speed;

	// DropTableID 설정 필요 시 저장
	DropTableID = MonsterData->DropTableID;

	UE_LOG(LogTemp, Log, TEXT("[AI StaticData] %s : (HP=%f, Atk=%f, Def=%f)"),
		*GetName(), MonsterData->MaxHealth, MonsterData->Attack, MonsterData->Armor);
}
