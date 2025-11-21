#include "SKInteractableBase.h"

#include <Component/InventoryComponent.h>
#include <GameData/SKGameConstant.h>
#include <Utility/SKGameplayMessageSubsystem.h>
#include <Utility/SKGameplayMessageTypes.h>

#include "Character/SKPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Controller/SKPlayerController.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

ASKInteractableBase::ASKInteractableBase()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	TraceCollision = CreateDefaultSubobject<UBoxComponent>("TraceCollision");
	TraceCollision->SetupAttachment(Root);

	TraceCollision->SetBoxExtent(FVector(50.0f));
	TraceCollision->SetHiddenInGame(false);
	TraceCollision->SetCollisionProfileName(TEXT("Interact"));
	TraceCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));
	TraceCollision->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.5f));
	
	InteractionPoint = CreateDefaultSubobject<USceneComponent>("InteractionPoint");
	InteractionPoint->SetupAttachment(Root);

	bReplicates = true;
}

void ASKInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);
	
	InteractionData.InteractionLocation = InteractionPoint->GetComponentLocation();
	InteractionData.InteractionRotation = InteractionPoint->GetComponentRotation();
}

void ASKInteractableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASKInteractableBase::AddToInventory(AActor* Interactor, int32 ItemID, int32 ItemQuantity)
{
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
