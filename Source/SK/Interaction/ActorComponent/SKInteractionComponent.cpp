#include "SKInteractionComponent.h"

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Item/SKInteractableBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Item/Openable/SKOpenableBase.h"

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
		if (!Actor->bCanInteract) continue;
		
		FVector ActorLocation = Actor->GetActorLocation();

		// 두 벡터 내적이 0보다 큰지
		FVector ToActor = (ActorLocation - OwnerLocation).GetSafeNormal();
		FVector ToOwner = (OwnerLocation - ActorLocation).GetSafeNormal();
		float Dot = 0;
		switch (Actor->ObjectType)
		{
		case EObjectType::NPC:
		case EObjectType::Pickup:
			Dot = FVector::DotProduct(ToActor, OwnerForwardVector);
			break;
		case EObjectType::Stool:
			if (FVector::DotProduct(Actor->GetActorRightVector(), ToOwner) > 0.3)
			{
				Dot = FVector::DotProduct(ToActor, OwnerForwardVector);
			}
			break;
		case EObjectType::Openable:
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
		UWidgetComponent* NewWidget = CurrentTargetActor->InteractionWidget;
		if (NewWidget)
		{
			Client_ToggleInteractableWidget(NewWidget,bIsVisible);
		}
		// 근접 UI 표시 해제
		if (CurrentTargetActor->bCanInteract)
		{
			ASKOpenableBase* OpenableTarget = Cast<ASKOpenableBase>(CurrentTargetActor);
			if (!OpenableTarget) return;
	
			UWidgetComponent* DetectWidget = OpenableTarget->DetectWidget;
			if (DetectWidget)
			{
				Client_ToggleInteractableWidget(DetectWidget, !bIsVisible);
			}
		}
	}
}

void USKInteractionComponent::OnRep_CurrentInteractionData()
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;

	if (Char->IsLocallyControlled())
	{
		Server_ActivateInteractionAbility(CurrentInteractionData.GrantedAbility);
	}
}

void USKInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USKInteractionComponent::Server_ActivateInteractionAbility_Implementation(TSubclassOf<UGameplayAbility> Ability) 
{
	ActivateInteractionAbility(Ability);
}

void USKInteractionComponent::ActivateInteractionAbility(TSubclassOf<UGameplayAbility> Ability) const
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
	if (!Char) return;

	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (!ASC) return;

	// FGameplayTagContainer InteractionTag;
	// InteractionTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.SimpleInteract")));
	// ASC->TryActivateAbilitiesByTag(InteractionTag);
	ASC->TryActivateAbilityByClass(Ability);
}

void USKInteractionComponent::Client_ToggleInteractableWidget_Implementation(UWidgetComponent* Widget, bool bIsVisible)
{
	if (Widget)
	{
		Widget->SetVisibility(bIsVisible);
	}
}

void USKInteractionComponent::Server_TryInteract_Implementation()
{
	if (!CurrentTargetActor) return;
	UE_LOG(LogTemp, Warning, TEXT("Role: %d"), CurrentTargetActor->GetLocalRole());
	UE_LOG(LogTemp, Warning, TEXT("Remote: %d"), CurrentTargetActor->GetRemoteRole());

	// Pickup은 바로 발동
	if (CurrentTargetActor->ObjectType == EObjectType::Pickup
		|| CurrentTargetActor->ObjectType == EObjectType::NPC)
	{
		ISKInteractable::Execute_Interact(CurrentTargetActor, GetOwner());
	}

	// Openable은 어빌리티 발동
	else
	{
		ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetOwner());
		if (!Char) return;

		ISKInteractable::Execute_GetInteractionData(CurrentTargetActor, CurrentInteractionData);
		
		// 서버는 바로 실행 서버에 복제 된 클라는 클라에 도착하면 서버 RPC로 실행
	
		// if (!GetOwner()->HasAuthority()) return;

		if (!Char->IsLocallyControlled()) return;

		// APlayerController* PC = Cast<APlayerController>(Char->GetController());
		// if (!PC || !PC->IsLocalController()) return;

		ActivateInteractionAbility(CurrentInteractionData.GrantedAbility);
	}

}