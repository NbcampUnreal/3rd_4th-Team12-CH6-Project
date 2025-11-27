#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SK_AnimNotify_AI_Snapshot.generated.h"

UCLASS()
class SK_API USK_AnimNotify_AI_Snapshot : public UAnimNotify
{
	GENERATED_BODY()

public:
	USK_AnimNotify_AI_Snapshot();
	
protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
