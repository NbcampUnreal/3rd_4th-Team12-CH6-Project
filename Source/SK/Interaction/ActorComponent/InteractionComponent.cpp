#include "InteractionComponent.h"

#include "Character/SKPlayerCharacter.h"
#include "GameData/SKGameConstant.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bShouldUseInteractionTrace = true;
	bIsActivate = false;

	InteractionTraceTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.InteractionTrace"));
	StateActionTag = FGameplayTag::RequestGameplayTag(TEXT("State.Action"));

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
	UpdateInteractionTrace();
	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetOwner());
	if (!SKPlayerCharacter->IsLocallyControlled())
	{
		UpdateInteractionTrace();
	}
}

void UInteractionComponent::UpdateInteractionTrace()
{
	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetOwner());
	if (!SKPlayerCharacter) return;
	
	UAbilitySystemComponent* ASC = SKPlayerCharacter->GetAbilitySystemComponent();
	if (!ASC) return;

	bShouldUseInteractionTrace = ASC->HasMatchingGameplayTag(StateActionTag) ? false : true;
	if (bShouldUseInteractionTrace && !bIsActivate)
	{
		if (ASC->FindAbilitySpecFromInputID(SKConstant::GA_Interact_ID))
		{
			bIsActivate = true;
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InteractionTraceTag));
		}
	}
	else if (!bShouldUseInteractionTrace && bIsActivate)
	{
		bIsActivate = false;
		FGameplayTagContainer InteractTagContainer(InteractionTraceTag);
		ASC->CancelAbilities(&InteractTagContainer);
	}
}

void UInteractionComponent::Server_GiveAndActivateAbility_Implementation(TSubclassOf<UGameplayAbility> AbilityClass, int32 AbilityLevel, int32 InputID)
{
	if (!AbilityClass) return;
	
	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetOwner());
	if (!SKPlayerCharacter) return;
	
	UAbilitySystemComponent* ASC = SKPlayerCharacter->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayAbilitySpecHandle NewHandle = ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, AbilityLevel, InputID, this));
	if (NewHandle.IsValid())
	{
		TWeakObjectPtr<UAbilitySystemComponent> WeakASC = ASC;
		FTimerHandle TempHandle;
		ASC->GetWorld()->GetTimerManager().SetTimer(TempHandle, [WeakASC, NewHandle]()
		{
			if (WeakASC.IsValid())
			{
				WeakASC->TryActivateAbility(NewHandle);
			}
		}, 0.01f, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Handle Is Invalid"));
	}
}

void UInteractionComponent::Server_CancelAbility_Implementation(const FGameplayAbilitySpecHandle Handle)
{
	ASKPlayerCharacter* SKPlayerCharacter = Cast<ASKPlayerCharacter>(GetOwner());
	if (!SKPlayerCharacter) return;
	
	UAbilitySystemComponent* ASC = SKPlayerCharacter->GetAbilitySystemComponent();
	if (ASC && Handle.IsValid())
	{
		ASC->ClearAbility(Handle);
	}
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