#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SK_AnimNotify_AI_SendEventToASC.generated.h"

UCLASS()
class SK_API USK_AnimNotify_AI_SendEventToASC : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag")
	FGameplayTag EventTag;

	
public:
	USK_AnimNotify_AI_SendEventToASC();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
