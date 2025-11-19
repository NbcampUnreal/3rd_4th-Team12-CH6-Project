#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "SKAICharacterBase.generated.h"

class USKAIAttributeSet;
class USKAIDataAsset;

UCLASS()
class SK_API ASKAICharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	USKAIAttributeSet* AttributeSet;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSoftObjectPtr<USKAIDataAsset> AIDataAsset;
	
public:
	ASKAICharacterBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void InitializeAttributeSetFromDataAsset();
};
