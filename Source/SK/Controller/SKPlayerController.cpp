// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/SKPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "ShaderPrintParameters.h"
#include "Character/SKCharacterBase.h"
#include "Character/SKPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Utility/SKUIManagerSubSystem.h"

ASKPlayerController::ASKPlayerController()
{
}

void ASKPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		check(DefaultMappingContext);
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	//다른 방법 있으면 추후 변경 예정 현재는 기능 테스트 용으로 추가
	USKUIManagerSubSystem* UISubSystem = ULocalPlayer::GetSubsystem<USKUIManagerSubSystem>(GetLocalPlayer());
	if (!UISubSystem) return;

	UISubSystem->SettingLayout();
}

void ASKPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		check(MoveAction);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                   &ASKPlayerController::StopJumping);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ASKPlayerController::Dash);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this,
		                                   &ASKPlayerController::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
		                                   &ASKPlayerController::StopSprint);

		EnhancedInputComponent->BindAction(NormalMeleeAttack, ETriggerEvent::Started, this,
								   &ASKPlayerController::NormalMelee);

		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Started, this,
							   &ASKPlayerController::InterAct);
	}
}

void ASKPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("ASKPlayerController::OnPossess"));	
	OnPawnPossessed.Broadcast(InPawn);;
}

void ASKPlayerController::Dash(const FInputActionValue& Value)
{
	ASKCharacterBase* SKChar = Cast<ASKCharacterBase>(GetPawn());
	if (!SKChar)
		return;
	UAbilitySystemComponent* ASC = SKChar->GetAbilitySystemComponent();
	if (!ASC)
		return;


	FGameplayTagContainer DashTag;
	DashTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Dash")));

	ASC->TryActivateAbilitiesByTag(DashTag);
}

void ASKPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InMoveVector = Value.Get<FVector2D>();

		const FRotator ControlrRotation = GetControlRotation();
		const FRotator ControlYawRotation(0.f, ControlrRotation.Yaw, 0.f);

		const FVector InLookVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
		const FVector InRightVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);


		ControlledPawn->AddMovementInput(InLookVector, InMoveVector.X);
		ControlledPawn->AddMovementInput(InRightVector, InMoveVector.Y);
	}
}

void ASKPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D InLookVector = Value.Get<FVector2D>();

	AddYawInput(InLookVector.X);
	AddPitchInput(InLookVector.Y);
}

void ASKPlayerController::Jump(const FInputActionValue& Value)
{
	if (ACharacter* pCharacter = Cast<ACharacter>(GetPawn()))
	{
		pCharacter->Jump();
	}
}

void ASKPlayerController::StopJumping()
{
	if (ACharacter* pCharacter = Cast<ACharacter>(GetPawn()))
	{
		pCharacter->StopJumping();
	}
}

void ASKPlayerController::StartSprint(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
		return;

	FGameplayTag SprintTag = FGameplayTag::RequestGameplayTag(FName("Ability.Sprint"));
	FGameplayTagContainer SprintTagContainer;
	SprintTagContainer.AddTag(SprintTag);

	ASC->TryActivateAbilitiesByTag(SprintTagContainer);
}

void ASKPlayerController::StopSprint(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
		return;

	FGameplayTagContainer SprintTagContainer;
	SprintTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Sprint")));

	ASC->CancelAbilities(&SprintTagContainer);
	

}

void ASKPlayerController::NormalMelee(const FInputActionValue& Value)
{
	
}

void ASKPlayerController::InterAct(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Interact"));
	
	ASKCharacterBase* SKChar = Cast<ASKCharacterBase>(GetPawn());
	if (!SKChar)
		return;
	UAbilitySystemComponent* ASC = SKChar->GetAbilitySystemComponent();
	if (!ASC)
		return;
	
	FGameplayTagContainer InteractionTag;
	InteractionTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Interact")));

	ASC->TryActivateAbilitiesByTag(InteractionTag);
}
