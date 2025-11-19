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
	// ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(GetOwningActorFromActorInfo());
	// FVector Start = SKCharacter->GetActorLocation();
	//ASC위치 수정으로 인해 아래로 수정
	ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!SKCharacter)
		return;

	FVector Start = SKCharacter->GetActorLocation();

	UCameraComponent* CameraComponent = SKCharacter->GetFollowCamera();
	FVector Direction = CameraComponent->GetForwardVector();
	Direction.Z = 0.f;
	Direction.Normalize();

	FVector End = Start + Direction * 200.0f;

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(SKCharacter);
	
	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(USKInteractable::StaticClass()))
		{
			CurrentHitActor = HitActor;
			// UI 정보 가져와야함
			// UI 띄우기
			ASKPickupItem* Item = Cast<ASKPickupItem>(HitActor);
			if (Item && Item->GetInteractionWidgetComponent())
			{
				Item->GetInteractionWidgetComponent()->SetVisibility(true);
			}
		}
		else
		{
			CurrentHitActor = nullptr;
			// UI 제거
			ASKPickupItem* Item = Cast<ASKPickupItem>(HitActor);
			if (Item && Item->GetInteractionWidgetComponent())
			{
				Item->GetInteractionWidgetComponent()->SetVisibility(false);
			}
		}
	}
	else
	{
		if (CurrentHitActor)
		{
			ASKPickupItem* PrevItem = Cast<ASKPickupItem>(CurrentHitActor);
			if (PrevItem && PrevItem->GetInteractionWidgetComponent())
			{
				PrevItem->GetInteractionWidgetComponent()->SetVisibility(false);
			}
			CurrentHitActor = nullptr;
		}
	}
}

void USK_GA_Interact::TryInteract()
{
	if (!CurrentHitActor) return;
	
	ASKPlayerCharacter* SKCharacter = Cast<ASKPlayerCharacter>(GetOwningActorFromActorInfo());
	
	FSKInteractionData InteractionData;

	// 상호작용 데이터 가져오기
	ISKInteractable::Execute_GetInteractionData(CurrentHitActor, InteractionData);
	// 대상 오브젝트 상호작용 시작
	ISKInteractable::Execute_Interact(CurrentHitActor, SKCharacter);
	
	UAbilitySystemComponent* ASC = SKCharacter->GetAbilitySystemComponent();
	if (!ASC) return;

	// 캐릭터 쪽 상호작용 실행
	if (InteractionData.GrantedAbility)
	{
		FGameplayAbilitySpecHandle NewHandle = ASC->GiveAbility(
			FGameplayAbilitySpec(InteractionData.GrantedAbility, 1, INDEX_NONE, this)
			);
		ASC->TryActivateAbility(NewHandle);
	}
}
