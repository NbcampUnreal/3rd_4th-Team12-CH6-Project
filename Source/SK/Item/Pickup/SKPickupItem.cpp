#include "SKPickupItem.h"

#include "Data/SKPickupItemData.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

ASKPickupItem::ASKPickupItem()
{
	bReplicates = true;
	
	UE_LOG(LogTemp, Warning, TEXT("[PickupItem] ASKPickupItem()"));
	ItemNiagara = CreateDefaultSubobject<UNiagaraComponent>("ItemNiagara");
	ItemNiagara->SetupAttachment(Root);
}

void ASKPickupItem::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[PickupItem] BeginPlay()"));
}

void ASKPickupItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASKPickupItem, PickupData);
	DOREPLIFETIME(ASKPickupItem, ItemCount);
}

void ASKPickupItem::InitializePickup(USKPickupItemData* InPickupData, int32 Count)
{
	PickupData = InPickupData;
	ItemCount = Count;

	if (PickupData->DropEffect)
	{
		ItemNiagara->SetAsset(PickupData->DropEffect);
		ItemNiagara->Activate(true);
	}
}

void ASKPickupItem::OnRep_PickupData()
{
	if (PickupData)
	{
		InitializePickup(PickupData, ItemCount);
	}
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

	if (ItemNiagara)
	{
		ItemNiagara->Deactivate();
		ItemNiagara->DestroyComponent();
		ItemNiagara = nullptr;
	}
	
	Destroy();
}