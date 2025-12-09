#include "SKInteractableBase.h"

#include <Component/InventoryComponent.h>
#include <Utility/SKGameplayMessageSubsystem.h>
#include <Utility/SKGameplayMessageTypes.h>

#include "Character/SKPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Controller/SKPlayerController.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"
#include "Interaction/UI/SKInteractableWidget.h"

ASKInteractableBase::ASKInteractableBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	InteractionCollision = CreateDefaultSubobject<USphereComponent>("Interaction");
	InteractionCollision->SetupAttachment(Root);

	// InteractionCollision->SetHiddenInGame(false);
	
	InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &ASKInteractableBase::OnOverlapBegin);
	InteractionCollision->OnComponentEndOverlap.AddDynamic(this, &ASKInteractableBase::OnOverlapEnd);
	InteractionCollision->SetIsReplicated(true);
	
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>("InteractionWidget");
	InteractionWidget->SetupAttachment(Root);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetVisibility(false);
	
	bReplicates = true;
	bCanInteract = true;
}

void ASKInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);

	if (!InteractionUI) return;

	if (USKInteractableWidget* WidgetClass = Cast<USKInteractableWidget>(InteractionUI))
	{
		InteractionWidget->SetWidget(WidgetClass);
		WidgetClass->SetInitialText(InteractableText);
	}
}

void ASKInteractableBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							  bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(OtherActor);
	if (!SKPlayerCharacter) return;
	
	USKInteractionComponent* InteractionComponent = SKPlayerCharacter->GetInteractionComponent();
	InteractionComponent->CandidateActors.Add(this);

}

void ASKInteractableBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;

	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(OtherActor);
	if (!SKPlayerCharacter) return;

	USKInteractionComponent* InteractionComponent = SKPlayerCharacter->GetInteractionComponent();
	InteractionComponent->CandidateActors.Remove(this);	

}

void ASKInteractableBase::PreExecuteInteraction()
{
	bCanInteract = false;
};

void ASKInteractableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASKInteractableBase, bCanInteract);
}

void ASKInteractableBase::AddToInventory(AActor* Interactor, int32 ItemID, int32 ItemQuantity)
{
	if (!HasAuthority()) return;
	
	if (!Interactor) return;

	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(Interactor);
	if (!SKPlayerCharacter) return;

	ASKPlayerController* SKPlayerController = Cast<ASKPlayerController>(SKPlayerCharacter->GetController());
	if (!SKPlayerController) return;

	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(SKPlayerController->PlayerState);
	if (!SKPlayerState) return;

	// 서버 권한 실행으로 수정
	UInventoryComponent* InventoryComponent = SKPlayerState->FindComponentByClass<UInventoryComponent>();
	InventoryComponent->AddItemByIDAndCount(ItemID, ItemQuantity);
}

void ASKInteractableBase::ToggleWidget(bool bIsVisible)
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(bIsVisible);
	}
}

void ASKInteractableBase::OnShowWidget(bool bIsVisible)
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{

			FSlotVisibilityMessage VisibilityMessage;
			VisibilityMessage.LayoutTag = TAG_UI_Layout_InGame;
			VisibilityMessage.SlotTags.AddTag(TAG_UI_Slot_Interaction);
			VisibilityMessage.bVisible = bIsVisible;

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible,	VisibilityMessage);
		}
	}
}

void ASKInteractableBase::GetInteractionData_Implementation(FSKInteractionData& OutData)
{
	OutData = InteractionData;
}