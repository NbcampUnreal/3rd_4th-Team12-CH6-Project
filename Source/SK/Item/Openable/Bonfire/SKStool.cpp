#include "SKStool.h"

#include "Character/SKPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"
#include "SKBonfire.h"
#include "GameData/SKGameConstant.h"

ASKStool::ASKStool()
{
	StoolMesh = CreateDefaultSubobject<UStaticMeshComponent>("Stool");
	StoolMesh->SetupAttachment(Root);

	StoolMesh->SetCollisionObjectType(SKConstant::ECC_Interactable);
	
	InteractionCollision->SetSphereRadius(150.0f);
	InteractionCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));

	DetectCollision->SetSphereRadius(300.f);
	
	ObjectType = EObjectType::Stool;
}

void ASKStool::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerBonfire = Cast<ASKBonfire>(GetAttachParentActor());
}

void ASKStool::ExecuteInteraction_Implementation(AActor* Interactor)
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Interactor);
	if (!PlayerCharacter) return;

	if (OwnerBonfire)
	{
		OwnerBonfire->ResetBonfire(PlayerCharacter);
	}
	
	ASKPlayerState* PS = PlayerCharacter->GetPlayerState<ASKPlayerState>();
	if (!PS) return;

	if (OwnerBonfire)
	{
		PS->CurrentBonfire = OwnerBonfire;
	}

	// UI 동작
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage Message(TAG_UI_Layout_BonfireMenu, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, Message);
		}
	}
}

void ASKStool::OverLapBeginHandle()
{
	if (OwnerBonfire)
	{
		TSet<UWidgetComponent*>& DetectWidgets = OwnerBonfire->DetectWidgets;
		if (DetectWidget)
		{
			DetectWidgets.Add(DetectWidget);
		}
	}
}

void ASKStool::OverLapEndHandle()
{
	if (OwnerBonfire)
	{
		TSet<UWidgetComponent*>& DetectWidgets = OwnerBonfire->DetectWidgets;
		if (DetectWidget)
		{
			DetectWidgets.Remove(DetectWidget);
		}
	}
}
