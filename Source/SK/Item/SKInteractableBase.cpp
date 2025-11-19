#include "SKInteractableBase.h"

#include <Utility/SKGameplayMessageSubsystem.h>
#include <Utility/SKGameplayMessageTypes.h>

#include "Character/SKPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Controller/SKPlayerController.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

ASKInteractableBase::ASKInteractableBase()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	TraceCollision = CreateDefaultSubobject<USphereComponent>("TraceCollision");
	TraceCollision->SetupAttachment(Root);

	TraceCollision->SetSphereRadius(100.0f);
	TraceCollision->SetHiddenInGame(false);
	// 전용 트레이스 채널 추가 필요
	TraceCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	InteractionPoint = CreateDefaultSubobject<USceneComponent>("InteractionPoint");
	InteractionPoint->SetupAttachment(Root);
}

void ASKInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionData.InteractionLocation = InteractionPoint->GetComponentLocation();
	InteractionData.InteractionRotation = InteractionPoint->GetComponentRotation();
}

void ASKInteractableBase::AddToInventory(AActor* Interactor, FName ItemName, int32 ItemQuantity)
{
	if (!Interactor) return;

	ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(Interactor);
	if (!SKCharacter) return;

	ASKPlayerController* SKController = Cast<ASKPlayerController>(Interactor);
	if (!SKController) return;

	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(Interactor);
	if (!SKPlayerState) return;

	// 수정 필요
	// USKInventoryComponent* InventoryComponent = Cast<USKInventoryComponent>(SKPlayerState->);
	// InventoryComponent->AddItem(ItemID, ItemQuantity);
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
