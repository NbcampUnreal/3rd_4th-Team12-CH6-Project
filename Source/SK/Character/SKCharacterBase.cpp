// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASKCharacterBase::ASKCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false; // 당장은 false

	// ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // or Full

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<USKAttributeSet>(TEXT("AttributeSet"));

	
}

UAbilitySystemComponent* ASKCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASKCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ASKCharacterBase, CharacterData, COND_InitialOnly);
}

void ASKCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

// Called when the game starts or when spawned
void ASKCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//충돌이나 속도,운동관련
	BaseSetting();
}

void ASKCharacterBase::BaseSetting()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

void ASKCharacterBase::SetDAPlayerStat()
{
	if (!CharacterData.Get())
		CharacterData.LoadSynchronous();
	
	if (!HasAuthority() || !CharacterData.Get())
		return;
	// AttributeSet의 초기값을 데이터 에셋의 값으로 설정
	AttributeSet->SetSpeed(CharacterData->Speed);
	AttributeSet->SetMaxHealth(CharacterData->MaxHealth);
	AttributeSet->SetHealth(CharacterData->Health);
	AttributeSet->SetMaxStamina(CharacterData->MaxStamina);
	AttributeSet->SetStamina(CharacterData->Stamina);
	AttributeSet->SetHeat(CharacterData->Heat);
	AttributeSet->SetMaxHeat(CharacterData->MaxHeat);
	AttributeSet->SetExp(CharacterData->Exp);
	AttributeSet->SetLevel(CharacterData->Level);
	AttributeSet->SetGold(CharacterData->Gold);
	AttributeSet->SetAttack(CharacterData->Attack);
	AttributeSet->SetArmor(CharacterData->Armor);
	AttributeSet->SetPoise(CharacterData->Poise);
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetSpeed();
	
		// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		// 	USKAttributeSet::GetSpeedAttribute()).AddUObject(this, &ASKCharacterBase::OnSpeedAttributeChanged);
	}
	
	//JobDataAsset - Give Ability
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterData->StartupAbilities)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0, this));
		}
	}
	
	//JobDataAsset - 팀태그 넘겨주는 코드
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

// Called every frame
void ASKCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASKCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

