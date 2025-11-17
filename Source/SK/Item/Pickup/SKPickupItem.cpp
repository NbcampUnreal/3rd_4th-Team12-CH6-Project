#include "SKPickupItem.h"

#include "Components/SphereComponent.h"
#include "Data/SKPickupItemData.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"

ASKPickupItem::ASKPickupItem()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	Staticmesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	Staticmesh->SetupAttachment(Root);

	TraceCollision = CreateDefaultSubobject<USphereComponent>("TraceCollision");
	TraceCollision->SetupAttachment(Root);

	TraceCollision->SetSphereRadius(100.0f);
	TraceCollision->SetHiddenInGame(false);
	// 전용 트레이스 채널 추가 필요
	TraceCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComponent"));
	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetVisibility(false);
}

void ASKPickupItem::BeginPlay()
{
	Super::BeginPlay();
}

void ASKPickupItem::PlayPickupSound()
{
	if (PickupData && PickupData->PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickupData->PickupSound,
			GetActorLocation()
		);
	}
}

UWidgetComponent* ASKPickupItem::GetInteractionWidgetComponent() const
{
	return InteractionWidgetComponent;
}
void ASKPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASKPickupItem::Interact_Implementation(AActor* Interactor)
{
	// UI
	// 인벤토리
	PlayPickupSound();
	Destroy();
}

void ASKPickupItem::GetInteractionData_Implementation(FSKInteractionData& OutData)
{
	OutData = InteractionData;
}
