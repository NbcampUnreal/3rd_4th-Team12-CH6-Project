#include "SKPickupItem.h"

#include "Data/SKPickupItemData.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ASKPickupItem::ASKPickupItem()
{
	UE_LOG(LogTemp, Warning, TEXT("[PickupItem] ASKPickupItem()"));
	ItemNiagara = CreateDefaultSubobject<UNiagaraComponent>("ItemNiagara");
	ItemNiagara->SetupAttachment(Root);

	InteractionCollision->SetSphereRadius(100.0f);
	InteractionCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
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
	DOREPLIFETIME(ASKPickupItem, ItemInfo);
}

void ASKPickupItem::InitializePickup(USKPickupItemData* InPickupData, int32 Count)
{
	PickupData = InPickupData;
	SetItemInfo(1, Count);

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
		InitializePickup(PickupData, ItemInfo.ItemCount);
	}
}

void ASKPickupItem::Multicast_PlayPickupEffects_Implementation(AActor* Interactor)
{
	if (USoundBase* PickupSound = GetPickupSound())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, Interactor->GetActorLocation());
	}

	if (ItemNiagara)
	{
		ItemNiagara->Deactivate();
		ItemNiagara->DestroyComponent();
		ItemNiagara = nullptr;
	}

	// 아이템 관련 효과 여기서
}

USoundBase* ASKPickupItem::GetPickupSound() const
{
	if (PickupData && PickupData->PickupSound)
	{
		return PickupData->PickupSound;
	}
	return nullptr;
}

int32 ASKPickupItem::GetItemID() const
{
	if (PickupData && ItemInfo.ItemID)
	{
		return ItemInfo.ItemID;
	}
	return -1;
}

void ASKPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASKPickupItem::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) return;

	AddToInventory(Interactor, ItemInfo.ItemCount, ItemInfo.ItemCount);

	Multicast_PlayPickupEffects(Interactor);
	
	Destroy();
}