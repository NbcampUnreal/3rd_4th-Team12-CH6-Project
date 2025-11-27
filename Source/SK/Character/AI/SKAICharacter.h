#pragma once

#include "CoreMinimal.h"
#include "Character/AI/SKAICharacterBase.h"
#include "SKAICharacter.generated.h"

UCLASS()
class SK_API ASKAICharacter : public ASKAICharacterBase
{
	GENERATED_BODY()

public:
	ASKAICharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void BeginPlay() override;
};
