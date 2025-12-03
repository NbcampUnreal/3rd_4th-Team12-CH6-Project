#include "SKInteractionComponent.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Item/SKInteractableBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

USKInteractionComponent::USKInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
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
	if (!GetOwner()->HasAuthority()) return;

	if (Cast<ACharacter>(GetOwner())->GetMovementComponent()->IsFalling())
	{
		// 점프 중에는 UI 끄기
		SetInteractionUI(false);
		return;
	}
	
	if (CandidateActors.Num() == 0)
	{
		// 오버랩이 끝났는데 타겟이 남아있으면 UI 끄기
		SetInteractionUI(false);
		return;
	}
	
	float MaxDot = -1.f;
	float MinDist = FLT_MAX;
	ASKInteractableBase* MaxActor = nullptr;
		
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	const FVector OwnerForwardVector = GetOwner()->GetActorForwardVector();

	for (auto* Actor : CandidateActors)
	{
		FVector ActorLocation = Actor->GetActorLocation();

		// 두 벡터 내적이 0보다 큰지
		FVector ToActor = (ActorLocation - OwnerLocation).GetSafeNormal();
		float Dot = 0;
		switch (Actor->ObjectType)
		{
		case EObjectType::Pickup:
			Dot = FVector::DotProduct(ToActor, OwnerForwardVector);
			break;
		case EObjectType::Openable:
			FVector ToOwner = (OwnerLocation - ActorLocation).GetSafeNormal();
			if (FVector::DotProduct(Actor->GetActorRightVector(), ToOwner) > 0)
			{
				Dot = FVector::DotProduct(ToActor, OwnerForwardVector);
			}
			break;
		}
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
		Client_ToggleInteractableWidget(CurrentTargetActor, bIsVisible);
	}
}

void USKInteractionComponent::OnRep_CurrentInteractionData()
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;

	if (Char->IsLocallyControlled())
	{
		Server_ActivateInteractionAbility();
	}
}

void USKInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USKInteractionComponent::Server_ActivateInteractionAbility_Implementation()
{
	ActivateInteractionAbility();
}

void USKInteractionComponent::ActivateInteractionAbility() const
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;

	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayTagContainer InteractionTag;
	InteractionTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.SimpleInteract")));
	ASC->TryActivateAbilitiesByTag(InteractionTag);
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

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;

	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent) return;

	FSKInteractionData Data;
	ISKInteractable::Execute_GetInteractionData(CurrentTargetActor, Data);
	InteractionComponent->SetInteractionData(Data);

	// 서버는 바로 실행 서버에 복제 된 클라는 클라에 도착하면 서버 RPC로 실행
	
	// if (!GetOwner()->HasAuthority()) return;

	if (!Char->IsLocallyControlled()) return;

	// APlayerController* PC = Cast<APlayerController>(Char->GetController());
	// if (!PC || !PC->IsLocalController()) return;

	ActivateInteractionAbility();
}