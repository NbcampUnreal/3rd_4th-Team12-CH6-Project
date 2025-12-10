#include "SKBonfire.h"

#include "NiagaraComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"


ASKBonfire::ASKBonfire()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(Root);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComponent->SetupAttachment(MeshComponent);
	NiagaraComponent->SetRelativeScale3D(FVector(2.0f));
	NiagaraComponent->SetRelativeLocation(FVector(0.f, 0.f, 60.0f));
	
	InteractionCollision->SetSphereRadius(300.0f);
	InteractionCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	
	ObjectType = EObjectType::Fireplace;
}

void ASKBonfire::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASKBonfire::ExecuteInteraction_Implementation(AActor* Interactor)
{
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Interactor);
	if (!PlayerCharacter) return;
	
	ASKPlayerState* PS = PlayerCharacter->GetPlayerState<ASKPlayerState>();
	if (!PS) return;
	
	PS->CurrentBonfire = this;

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