#include "SKInteractionComponent.h"

#include "Character/SKPlayerCharacter.h"
#include "Net/UnrealNetwork.h"

USKInteractionComponent::USKInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USKInteractionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USKInteractionComponent, CurrentInteractionData);
	DOREPLIFETIME(USKInteractionComponent, CurrentTargetActor);
}


void USKInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

}


void USKInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

{
}

void USKInteractionComponent::Server_TryInteract_Implementation()
{
	if (!CurrentTargetActor) return;
	UE_LOG(LogTemp, Warning, TEXT("Role: %d"), CurrentTargetActor->GetLocalRole());
	UE_LOG(LogTemp, Warning, TEXT("Remote: %d"), CurrentTargetActor->GetRemoteRole());
	ISKInteractable::Execute_Interact(CurrentTargetActor, GetOwner());
}