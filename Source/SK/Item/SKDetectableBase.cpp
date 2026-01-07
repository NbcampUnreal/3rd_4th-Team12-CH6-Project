#include "SKDetectableBase.h"

#include "Character/SKPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"
#include "Weapon/ActorComponent/SKActionComponent.h"

ASKDetectableBase::ASKDetectableBase()
{
	DetectCollision = CreateDefaultSubobject<USphereComponent>("DetectCollision");
	DetectCollision->SetupAttachment(Root);	
	DetectCollision->SetSphereRadius(800.0f);
	// DetectCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	// DetectCollision->SetCollisionProfileName(TEXT("Interact"));
	DetectCollision->OnComponentBeginOverlap.AddDynamic(this, &ASKDetectableBase::OnDetectBeginOverlap);
	DetectCollision->OnComponentEndOverlap.AddDynamic(this, &ASKDetectableBase::OnDetectEndOverlap);
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

void ASKDetectableBase::BeginPlay()
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

void ASKDetectableBase::OverLapBeginHandle()
{
}

void ASKDetectableBase::OverLapEndHandle()
{
}

void ASKDetectableBase::OnDetectBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanInteract) return;
	
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(OtherActor);
	if (!Character) return;

	USKActionComponent* ActionComponent = Character->GetActionComponent();
	if (!ActionComponent || ActionComponent->InteractedStool) return;

	USKInteractionComponent* InteractionComponent = Character->GetInteractionComponent();
	if (!InteractionComponent) return;
	
	InteractionComponent->DetectedWidgets.Add(DetectWidget);
	
	if (!Character->IsLocallyControlled()) return;

	OverlappedCharacter.Add(Character);
	
	DetectWidget->SetVisibility(true);

	OverLapBeginHandle();
}

void ASKDetectableBase::OnDetectEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bCanInteract) return;
	
	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(OtherActor);
	if (!Character) return;
	
	USKInteractionComponent* InteractionComponent = Character->GetInteractionComponent();
	if (!InteractionComponent) return;
	
	InteractionComponent->DetectedWidgets.Remove(DetectWidget);

	if (!Character->IsLocallyControlled()) return;
	
	OverlappedCharacter.Remove(Character);
	
	DetectWidget->SetVisibility(false);
	
	OverLapEndHandle();
}

void ASKDetectableBase::HandleCanInteractChanged()
{
	InteractionWidget->SetVisibility(false);
	DetectWidget->SetVisibility(false);
}

