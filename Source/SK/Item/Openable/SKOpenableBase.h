#pragma once

#include "CoreMinimal.h"
#include "Item/SKDetectableBase.h"
#include "SKOpenableBase.generated.h"

class USKOpenableData;

UCLASS()
class SK_API ASKOpenableBase : public ASKDetectableBase
{
	GENERATED_BODY()

public:
	ASKOpenableBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Data")
	TObjectPtr<USKOpenableData> OpenableData;
};
