#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/Interface/SKInteractable.h"
#include "InteractionComponent.generated.h"

class ASKPlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SK_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(Server, Reliable)
	void Server_TryInteract(AActor* Target);

	UFUNCTION(Client, Reliable)
	void Client_PlayPickupSound(USoundBase* PickupSound);

	UPROPERTY(BlueprintReadWrite, Replicated)
	FSKInteractionData CurrentInteractionData;

	FORCEINLINE void SetShouldUseInteractionTrace(const bool NewValue) { bShouldUseInteractionTrace = NewValue;}

	UFUNCTION(Server, Reliable)
	void Server_GiveAndActivateAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 AbilityLevel, int32 InputID);

	UFUNCTION(Server, Reliable)
	void Server_CancelAbility(const FGameplayAbilitySpecHandle Handle);

	FORCEINLINE FSKInteractionData& GetInteractionData() { return CurrentInteractionData; }
	
protected:
	UFUNCTION()
	void UpdateInteractionTrace();

private:
	bool bShouldUseInteractionTrace;
	bool bIsActivate;
	
	FGameplayTag InteractionTraceTag;
	FGameplayTag StateActionTag;

};
