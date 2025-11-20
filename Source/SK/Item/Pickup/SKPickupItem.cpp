#include "SKPickupItem.h"

#include "Data/SKPickupItemData.h"
#include "Kismet/GameplayStatics.h"

ASKPickupItem::ASKPickupItem()
{
	ItemNiagara = CreateDefaultSubobject<UNiagaraComponent>("ItemNiagara");
	ItemNiagara->SetupAttachment(Root);
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