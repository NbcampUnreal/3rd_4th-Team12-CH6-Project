#pragma once

#include "CoreMinimal.h"
#include "Character/AI/SKAICharacterBase.h"
#include "SKAICharacter.generated.h"

struct FGameplayEffectSpec;
class UWidgetComponent;

UCLASS()
class SK_API ASKAICharacter : public ASKAICharacterBase
{
	GENERATED_BODY()

public:
	ASKAICharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void BeginPlay() override;

	// 위젯 컴포넌트 2개
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* HealthWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* DamageWidgetComponent;

	void OnMonsterHealthChange(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);

	UFUNCTION()
	void UIWidgetComponentRotationChange();

private:
	FTimerHandle WidgetRotationTimerHandle;
	FTimerHandle WidgetRotationStopTimerHandle;
};
