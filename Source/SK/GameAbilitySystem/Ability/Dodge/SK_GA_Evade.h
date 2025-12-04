#pragma once

#include "CoreMinimal.h"
#include "SK_GA_Dodge.h"
#include "SK_GA_Evade.generated.h"

UCLASS()
class SK_API USK_GA_Evade : public USK_GA_Dodge
{
	GENERATED_BODY()

protected:
	virtual void PreActivateDodge(USKActionComponent* ActionComponent) override;
};
