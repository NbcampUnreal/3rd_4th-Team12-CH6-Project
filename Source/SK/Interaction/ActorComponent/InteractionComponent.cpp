#include "InteractionComponent.h"

#include "Character/SKPlayerCharacter.h"
#include "GameData/SKGameConstant.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractionComponent, CurrentInteractionData);
}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractionComponent::Client_PlayPickupSound_Implementation(USoundBase* PickupSound)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetOwner()->GetActorLocation());
}

void UInteractionComponent::Server_TryInteract_Implementation(AActor* Target)
{
	if (!Target) return;
	UE_LOG(LogTemp, Warning, TEXT("Role: %d"), Target->GetLocalRole());
	UE_LOG(LogTemp, Warning, TEXT("Remote: %d"), Target->GetRemoteRole());
	ISKInteractable::Execute_Interact(Target, GetOwner());
}