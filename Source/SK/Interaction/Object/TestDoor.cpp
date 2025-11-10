#include "TestDoor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

ATestDoor::ATestDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Root);

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(Root);
	InteractionBox->SetBoxExtent(FVector(100.0f));

	bIsOpen = false;
	OpenAngle = -90.f;
	OpenSpeed = 2.f;
}

void ATestDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialRotation = DoorMesh->GetRelativeRotation();
	TargetRotation = InitialRotation;
}

void ATestDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FRotator CurrentRotation = DoorMesh->GetRelativeRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, OpenSpeed);
	DoorMesh->SetRelativeRotation(NewRotation);
}

void ATestDoor::Interact_Implementation(AActor* Interactor)
{
	bIsOpen = !bIsOpen;

	if (bIsOpen)
	{
		TargetRotation = InitialRotation + FRotator(0.f, OpenAngle, 0.f);
	}
	else
	{
		TargetRotation = InitialRotation;
	}
}

void ATestDoor::GetInteractionData_Implementation(FInteractionData& OutData)
{
	OutData = InteractionData;
}

