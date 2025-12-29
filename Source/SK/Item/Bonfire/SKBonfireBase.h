#pragma once

#include "CoreMinimal.h"
#include "Item/SKDetectableBase.h"
#include "SKBonfireBase.generated.h"

class ASKBonfire;

UCLASS()
class SK_API ASKBonfireBase : public ASKDetectableBase
{
	GENERATED_BODY()

public:
	ASKBonfireBase();
	
	UPROPERTY()
	ASKBonfire* OwnerBonfire;

protected:
	virtual void OverLapBeginHandle() override;

	virtual void OverLapEndHandle() override;
};
