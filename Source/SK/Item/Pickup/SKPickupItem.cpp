#include "SKPickupItem.h"

#include "Character/SKPlayerCharacter.h"
#include "Data/SKPickupItemData.h"
#include "Net/UnrealNetwork.h"
#include "Interaction/ActorComponent/InteractionComponent.h"

ASKPickupItem::ASKPickupItem()
{
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
	if (PickupData && PickupData->ItemID)
	{
		return PickupData->ItemID;
	}
	return -1;
}

void ASKPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASKPickupItem::Interact_Implementation(AActor* Interactor)
{
	if (ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(Interactor))
	{
		UInteractionComponent* InteractionComponent = SKPlayerCharacter->GetInteractionComponent();
		InteractionComponent->Client_PlayPickupSound(GetPickupSound());
	}

	if (ItemNiagara)
	{
		ItemNiagara->Deactivate();
		ItemNiagara->DestroyComponent();
		ItemNiagara = nullptr;
	}

	AddToInventory(Interactor, GetItemID(), ItemCount);
	
	Destroy();
}