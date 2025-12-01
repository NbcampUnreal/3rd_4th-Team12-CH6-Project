#include "SKInteractionComponent.h"

#include "Character/SKPlayerCharacter.h"
#include "Controller/SKPlayerController.h"
#include "Item/SKInteractableBase.h"
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

	GetWorld()->GetTimerManager().SetTimer(
		UpdateTargetHandle,
		this,
		&USKInteractionComponent::UpdateTargetActor,
		UpdateInterval,
		true);
}

void USKInteractionComponent::UpdateTargetActor()
{
	
	if (CandidateActors.Num() == 0)
	{
		// 오버랩이 끝났는데 타겟이 남아있으면 UI 끄기
		SetInteractionUI(false);
		return;
	}
	
	float MaxDot = -1.f;
	float MinDist = INFINITY;
	AActor* MaxActor = nullptr;
		
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	const FVector OwnerForwardVector = GetOwner()->GetActorForwardVector();

	for (auto const Actor : CandidateActors)
	{
		FVector ActorLocation = Actor->GetActorLocation();

		// 두 벡터 내적이 0보다 큰지
		FVector ToActor = (ActorLocation - OwnerLocation).GetSafeNormal();
		float Dot = FVector::DotProduct(ToActor, OwnerForwardVector);
		if (Dot > 0)
		{
			FVector Distance = OwnerLocation - ActorLocation;
			float Dist = Distance.SquaredLength();
			if (Dot >= MaxDot)
			{
				MaxDot = Dot;
				MinDist = Dist;
				MaxActor = Actor;
			}
			else if (FMath::IsNearlyEqual(Dot, MaxDot) && Dist < MinDist)
			{
				MinDist = Dist;
				MaxActor = Actor;
			}
		}
			
	}
	// 이전 UI 끄기
	SetInteractionUI(false);
	
	CurrentTargetActor = MaxActor;

	// 새로운 타겟 UI 켜기
	SetInteractionUI(true);
	
}


void USKInteractionComponent::SetInteractionUI(const bool bIsVisible)
{
	if (IsValid(CurrentTargetActor))
	{
		ASKInteractableBase* Interactable = Cast<ASKInteractableBase>(CurrentTargetActor);
		if (Interactable)
		{
			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			if (!OwnerPawn) return;
 
			ASKPlayerController* PC = Cast<ASKPlayerController>(OwnerPawn->GetController());
			if (!PC) return;

			if (PC->HasAuthority())
			{
				Client_ToggleInteractableWidget(Interactable, bIsVisible);
			}
			else
			{
				Interactable->ToggleWidget(bIsVisible);
			}
		}
	}
}

void USKInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USKInteractionComponent::Client_ToggleInteractableWidget_Implementation(ASKInteractableBase* Interactable,
	bool bIsVisible)
{
	if (Interactable)
	{
		Interactable->ToggleWidget(bIsVisible);
	}
}

void USKInteractionComponent::Server_TryInteract_Implementation()
{
	if (!CurrentTargetActor) return;
	UE_LOG(LogTemp, Warning, TEXT("Role: %d"), CurrentTargetActor->GetLocalRole());
	UE_LOG(LogTemp, Warning, TEXT("Remote: %d"), CurrentTargetActor->GetRemoteRole());
	ISKInteractable::Execute_Interact(CurrentTargetActor, GetOwner());
}