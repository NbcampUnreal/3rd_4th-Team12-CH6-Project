#include "SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Controller/SKPlayerController.h"

USKActionComponent::USKActionComponent()
	: CurrentWeaponActionData(nullptr)
{
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

FRotator USKActionComponent::GetDodgeRotator()
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return FRotator();
	
	ASKPlayerController* PC = Cast<ASKPlayerController>(Char->GetController());
	if (!PC) return FRotator();

	const FRotator ControllerRot = PC->GetControlRotation();
	const FVector2D InputVector = PC->CurrentInputVector;

	FRotator TempRot = ControllerRot;
	TempRot.Roll = 0.f;
	TempRot.Pitch = 0.f;

	const FVector TempForward = FRotationMatrix(TempRot).GetUnitAxis(EAxis::X);
	const FVector TempRight = FRotationMatrix(TempRot).GetUnitAxis(EAxis::Y);

	const FVector TargetVector = TempForward * InputVector.X + TempRight * InputVector.Y;
	const FRotator TargetRot = TargetVector.GetSafeNormal().Rotation();
	
	return TargetRot;
}

