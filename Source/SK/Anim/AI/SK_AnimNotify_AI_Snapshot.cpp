#include "Anim/AI/SK_AnimNotify_AI_Snapshot.h"

USK_AnimNotify_AI_Snapshot::USK_AnimNotify_AI_Snapshot()
{
	
}

void USK_AnimNotify_AI_Snapshot::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	MeshComp->GetAnimInstance()->SavePoseSnapshot(TEXT("PoseSnapshot"));
	//활용 가능성 큼, 아직 미적용
}
