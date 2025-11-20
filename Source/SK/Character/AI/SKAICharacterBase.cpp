#include "Character/AI/SKAICharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"
#include "SKAIDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ASKAICharacterBase::ASKAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // or Full

	AttributeSet = CreateDefaultSubobject<USKAIAttributeSet>(TEXT("AttributeSet"));
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
	if (!AIDataAsset.Get())
	{
		AIDataAsset.LoadSynchronous();
	}

	if (!HasAuthority() || !AIDataAsset.Get())
	{
		return;
	}

	// 직접 말고 GE로 초기화 해보기
	AttributeSet->SetHealth(AIDataAsset->Health);
	AttributeSet->SetMaxHealth(AIDataAsset->MaxHealth);
	AttributeSet->SetAttack(AIDataAsset->Attack);
	AttributeSet->SetArmor(AIDataAsset->Armor);
	AttributeSet->SetPoise(AIDataAsset->Poise);
	AttributeSet->SetSpeed(AIDataAsset->Speed);
	AttributeSet->SetSprintWeight(AIDataAsset->SprintWeight);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetSpeed();
	}

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
