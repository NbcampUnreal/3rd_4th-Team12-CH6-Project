#include "SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Controller/SKPlayerController.h"
#include "Animation/SKPlayerAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"

USKActionComponent::USKActionComponent()
	: CurrentWeaponAnimData(nullptr)
{
	SetIsReplicatedByDefault(true);
}

void USKActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USKActionComponent, CurrentInputVector);
	DOREPLIFETIME(USKActionComponent, CurrentMoveDirection);
}

void USKActionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void USKActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USKActionComponent::Multicast_SetWeaponAnimData_Implementation(USKWeaponAnimData* NewWeaponAnimData)
{
	CurrentWeaponAnimData = NewWeaponAnimData;
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;
	
	Char->GetMesh()->SetAnimInstanceClass(CurrentWeaponAnimData->AnimInstance);
	
}

void USKActionComponent::Server_SetMovementInfo_Implementation(const FVector2D NewInputVector, const EMoveDirection NewMoveDirection)
{
	CurrentInputVector = NewInputVector;
	CurrentMoveDirection = NewMoveDirection;

	SetMoveDirection();
}

void USKActionComponent::OnRep_OnMoveDirectionChange()
{
	SetMoveDirection();
}

void USKActionComponent::SetMoveDirection()
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;
	
	USKPlayerAnimInstance* PlayerAnimInstance = Cast<USKPlayerAnimInstance>(Char->GetMesh()->GetAnimInstance());
	if (PlayerAnimInstance)
	{
		PlayerAnimInstance->CurrentMoveDirection = CurrentMoveDirection;
	}
}


FRotator USKActionComponent::GetDodgeRotator() const
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return FRotator();
	
	ASKPlayerController* PC = Cast<ASKPlayerController>(Char->GetController());
	if (!PC) return FRotator();

	const FRotator ControllerRot = PC->GetControlRotation();
	FVector2D InputVector = CurrentInputVector;
	FRotator TempRot = ControllerRot;
	TempRot.Roll = 0.f;
	TempRot.Pitch = 0.f;

	const FVector TempForward = FRotationMatrix(TempRot).GetUnitAxis(EAxis::X);
	const FVector TempRight = FRotationMatrix(TempRot).GetUnitAxis(EAxis::Y);

	const FVector TargetVector = TempForward * InputVector.X + TempRight * InputVector.Y;
	const FRotator TargetRot = TargetVector.GetSafeNormal().Rotation();
	UE_LOG(LogTemp, Warning, TEXT("TargetRotation, %s"), *TargetRot.ToString())
	return TargetRot;
}

bool USKActionComponent::CheckDoubleTap()
{
	bool bIsDoubleTap = false;
	constexpr float TabDelay = 0.2f;
	
	if (bIsGateOpen)
	{
		bIsDoubleTap = true;
	}

	bIsGateOpen = true;
	GetWorld()->GetTimerManager().ClearTimer(GateTimerHandle);
	
	GetWorld()->GetTimerManager().SetTimer(
		GateTimerHandle,
		this,
		&USKActionComponent::CloseGate,
		TabDelay,
		false);
	
	return bIsDoubleTap;
}

void USKActionComponent::CloseGate()
{
	bIsGateOpen = false;
}

void USKActionComponent::Server_ExecuteDodge_Implementation(FName DodgeTag)
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;

	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayTagContainer StepTag;
	StepTag.AddTag(FGameplayTag::RequestGameplayTag(DodgeTag));
	ASC->TryActivateAbilitiesByTag(StepTag);
}

void USKActionComponent::TryDodge()
{
	bool bIsEvade = CheckDoubleTap();
	FName DodgeTag;
	if (!bIsEvade)
	{
		DodgeTag = FName("State.Action.Dodge.Step");
	}
	else
	{
		DodgeTag = FName("State.Action.Dodge.Evade");
	}
	
	Server_ExecuteDodge(DodgeTag);
}