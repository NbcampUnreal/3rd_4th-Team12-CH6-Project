#include "SKOpenableBase.h"

#include "Character/SKPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

ASKOpenableBase::ASKOpenableBase()
{
	DetectCollision = CreateDefaultSubobject<USphereComponent>("DetectCollision");
	DetectCollision->SetupAttachment(Root);	
	DetectCollision->SetSphereRadius(800.0f);
	// DetectCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	// DetectCollision->SetCollisionProfileName(TEXT("Interact"));
	DetectCollision->OnComponentBeginOverlap.AddDynamic(this, &ASKOpenableBase::OnDetectBeginOverlap);
	DetectCollision->OnComponentEndOverlap.AddDynamic(this, &ASKOpenableBase::OnDetectEndOverlap);
	DetectCollision->SetIsReplicated(true);

	InteractionCollision->SetSphereRadius(100.0f);
	InteractionCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	
	InteractionPoint = CreateDefaultSubobject<UCapsuleComponent>("InteractionPoint");
	InteractionPoint->SetupAttachment(Root);
	InteractionPoint->SetCapsuleSize(34.f, 88.f);
	InteractionPoint->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	InteractionPoint->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	
	ObjectType = EObjectType::Openable;

	DetectWidget = CreateDefaultSubobject<UWidgetComponent>("DetectWidget");
	DetectWidget->SetupAttachment(Root);
	DetectWidget->SetWidgetSpace(EWidgetSpace::Screen);
	DetectWidget->SetVisibility(false);
}

void ASKOpenableBase::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionData.InteractionLocation = InteractionPoint->GetComponentLocation();
	InteractionData.InteractionRotation = InteractionPoint->GetComponentRotation();

	
	if (!DetectWidgetClass) return;

	if (DetectWidgetClass)
	{
		DetectWidget->SetWidget(DetectWidgetClass);
	}
}

void ASKOpenableBase::OverLapBeginHandle()
{
}

void ASKOpenableBase::OverLapEndHandle()
{
}

void ASKOpenableBase::OnDetectBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanInteract) return;
	
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(OtherActor);
	if (!Char) return;

	USKActionComponent* ActionComponent = Char->GetActionComponent();
	if (!ActionComponent || ActionComponent->InteractedStool) return;
	
	if (!Char->IsLocallyControlled()) return;
	
	DetectWidget->SetVisibility(true);

	OverLapBeginHandle();
}

void ASKOpenableBase::OnDetectEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bCanInteract) return;
	
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(OtherActor);
	if (!Char) return;

	if (!Char->IsLocallyControlled()) return;
	
	DetectWidget->SetVisibility(false);
	
	OverLapEndHandle();
}

void ASKOpenableBase::HandleCanInteractChanged()
{
	InteractionWidget->SetVisibility(false);
	DetectWidget->SetVisibility(false);
}

