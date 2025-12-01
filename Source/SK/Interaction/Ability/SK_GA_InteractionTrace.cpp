#include "SK_GA_InteractionTrace.h"

#include <GameData/SKGameConstant.h>

#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Interaction/Interface/SKInteractable.h"
#include "Item/Pickup/SKPickupItem.h"
#include "PlayerState/SKPlayerState.h"
#include "Interaction/ActorComponent/InteractionComponent.h"


USK_GA_InteractionTrace::USK_GA_InteractionTrace()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USK_GA_InteractionTrace::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (AActor* Owner = GetAvatarActorFromActorInfo())
	{
		Owner->GetWorldTimerManager().SetTimer(
			TraceTimerHandle,
			this,
			&USK_GA_InteractionTrace::LineTraceWithChannel,
			0.033f,
			true
		);
	}
}

void USK_GA_InteractionTrace::InputPressed(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	TryInteract();
}

void USK_GA_InteractionTrace::LineTraceWithChannel()
{
	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!SKPlayerCharacter) return;

	FVector Start = SKPlayerCharacter->GetActorLocation();
	
	UCameraComponent* CameraComponent = SKPlayerCharacter->GetFollowCamera();
	FVector Direction = CameraComponent->GetForwardVector();
	// Direction.Z = 0.f;
	// Direction.Normalize();

	FVector End = Start + Direction * 200.0f;

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(SKPlayerCharacter);

	AActor* OldActor = CurrentHitActor;
	
	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, SKConstant::ECC_Interactable, CollisionParams))
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

void USK_GA_InteractionTrace::TryInteract()
{
	if (!CurrentHitActor) return;
	
	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!SKPlayerCharacter) return;

	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(GetOwningActorFromActorInfo());
	if (!SKPlayerState) return;
	
	FSKInteractionData InteractionData;

	// 상호작용 데이터 가져오기
	ISKInteractable::Execute_GetInteractionData(CurrentHitActor, InteractionData);

	UInteractionComponent* InteractionComponent = SKPlayerCharacter->GetInteractionComponent();
	InteractionComponent->GetInteractionData() = InteractionData;

	// 대상 오브젝트 상호작용 시작
	InteractionComponent->Server_TryInteract(CurrentHitActor);

	// 캐릭터 쪽 상호작용 실행
	// InteractionComponent->Server_GiveAndActivateAbility(InteractionData.GrantedAbility, 1, INDEX_NONE);
}
