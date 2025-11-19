#include "SK_GA_Interact.h"

#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/SKPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Interaction/Interface/SKInteractable.h"
#include "Item/Pickup/SKPickupItem.h"


USK_GA_Interact::USK_GA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USK_GA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AActor* Owner = GetOwningActorFromActorInfo())
	{
		Owner->GetWorldTimerManager().SetTimer(
			TraceTimerHandle,
			this,
			&USK_GA_Interact::LineTraceWithChannel,
			0.033f,
			true
		);
	}
}

void USK_GA_Interact::InputPressed(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	TryInteract();
}

void USK_GA_Interact::LineTraceWithChannel()
{
	ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(GetOwningActorFromActorInfo());
	FVector Start = SKCharacter->GetActorLocation();

	UCameraComponent* CameraComponent = SKCharacter->GetFollowCamera();
	FVector Direction = CameraComponent->GetForwardVector();
	Direction.Z = 0.f;
	Direction.Normalize();

	FVector End = Start + Direction * 200.0f;

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(SKCharacter);

	AActor* OldActor = CurrentHitActor;
	
	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(USKInteractable::StaticClass()))
		{
			CurrentHitActor = HitActor;
		}
		else
		{
			CurrentHitActor = nullptr;
		}
	}
	else
	{
		CurrentHitActor = nullptr;
	}
	
	if (OldActor && OldActor != CurrentHitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hide Widget"));
		if (ASKInteractableBase* OldItem = Cast<ASKInteractableBase>(OldActor))
			OldItem->OnShowWidget(false);
	}

	if (CurrentHitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Show Widget"));
		if (ASKInteractableBase* NewItem = Cast<ASKInteractableBase>(CurrentHitActor))
			NewItem->OnShowWidget(true);
	}
	
}

void USK_GA_Interact::TryInteract()
{
	if (!CurrentHitActor) return;
	
	ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(GetOwningActorFromActorInfo());
	if (!SKCharacter) return;
	
	FSKInteractionData InteractionData;

	// 상호작용 데이터 가져오기
	ISKInteractable::Execute_GetInteractionData(CurrentHitActor, InteractionData);
	SKCharacter->CurrentInteractionData = InteractionData;

	// 대상 오브젝트 상호작용 시작
	ISKInteractable::Execute_Interact(CurrentHitActor, SKCharacter);
	
	UAbilitySystemComponent* ASC = SKCharacter->GetAbilitySystemComponent();
	if (!ASC) return;
	
	// 캐릭터 쪽 상호작용 실행
	if (InteractionData.GrantedAbility)
	{
		UE_LOG(LogTemp, Log, TEXT("TryActivate Ability: %s"), *InteractionData.GrantedAbility->GetName())
		FGameplayAbilitySpecHandle NewHandle = ASC->GiveAbility(
			FGameplayAbilitySpec(InteractionData.GrantedAbility, 1, INDEX_NONE, this)
			);

		if (NewHandle.IsValid())
		{
			FTimerHandle TempHandle;
			SKCharacter->GetWorldTimerManager().SetTimer(TempHandle, [ASC, NewHandle]()
			{
				ASC->TryActivateAbility(NewHandle);
			}, 0.01f, false);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Handle Is Invalid"));
		}
	}
}
