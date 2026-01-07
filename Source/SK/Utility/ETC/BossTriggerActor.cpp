#include "Utility/ETC/BossTriggerActor.h"


ABossTriggerActor::ABossTriggerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ABossTriggerActor::FireTrigger()
{
	
	if (!OnAITriggered.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossTriggerActor] Not Triggerd"));
		return;
	}

	OnAITriggered.Broadcast();
}

void ABossTriggerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnAITriggered.Clear();
	Super::EndPlay(EndPlayReason);
}