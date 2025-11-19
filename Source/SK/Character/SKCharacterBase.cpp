// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerState/SKPlayerState.h"

// Sets default values
ASKCharacterBase::ASKCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false; // 당장은 false
}

void ASKCharacterBase::InitASCFromPlayerState()
{
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	if (!PS)
		return;
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSet = PS->GetAttributeSet();

	if (!AbilitySystemComponent)
		return;

	AbilitySystemComponent->InitAbilityActorInfo(PS, this);

	// Delegate 바인딩
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		USKAttributeSet::GetSpeedAttribute()
	).AddUObject(this, &ASKCharacterBase::OnSpeedAttributeChanged);

	// 초기 속도 적용
	OnSpeedAttributeChanged(FOnAttributeChangeData());

}


void ASKCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ASKCharacterBase, CharacterData, COND_InitialOnly);
}

void ASKCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitASCFromPlayerState();

}

void ASKCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitASCFromPlayerState(); // 클라
}

UAbilitySystemComponent* ASKCharacterBase::GetAbilitySystemComponent() const
{
	ASKPlayerState* PS = GetPlayerState<ASKPlayerState>();
	return PS ? PS->GetAbilitySystemComponent() : nullptr;
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

void ASKCharacterBase::OnSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!AttributeSet)
		return;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp == nullptr)
		return;

	// AttributeSet에서 Speed 값 읽기
	const float NewSpeed = AttributeSet->GetSpeed();

	// 이동 속도 적용
	MoveComp->MaxWalkSpeed = NewSpeed;
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
