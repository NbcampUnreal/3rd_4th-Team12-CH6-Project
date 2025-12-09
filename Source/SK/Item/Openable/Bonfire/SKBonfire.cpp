#include "SKBonfire.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"


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
	// ResetAttribute(); -> 구현 위치?
	// ResetMonster(); -> 구현 위치?
	
	// HUD 제거  -> 구현 위치?
	// 화톳불 전용 UI 표시 -> 구현 위치?
}