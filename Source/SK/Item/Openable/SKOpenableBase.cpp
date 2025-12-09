#include "SKOpenableBase.h"
#include "Components/SphereComponent.h"

ASKOpenableBase::ASKOpenableBase()
{
	PrimaryActorTick.bCanEverTick = true;
		
	OverlapCollision = CreateDefaultSubobject<USphereComponent>("OverlapCollision");
	OverlapCollision->SetupAttachment(Root);	
	OverlapCollision->SetSphereRadius(500.0f);
	OverlapCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	OverlapCollision->SetCollisionProfileName(TEXT("Interact"));

	InteractionCollision->SetSphereRadius(50.0f);
	InteractionCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	
	InteractionPoint = CreateDefaultSubobject<USceneComponent>("InteractionPoint");
	InteractionPoint->SetupAttachment(Root);
	
	ObjectType = EObjectType::Openable;
}

void ASKOpenableBase::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionData.InteractionLocation = InteractionPoint->GetComponentLocation();
	InteractionData.InteractionRotation = InteractionPoint->GetComponentRotation();
}

{
	Super::Tick(DeltaTime);
}

