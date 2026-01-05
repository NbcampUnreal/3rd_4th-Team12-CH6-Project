// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/SKAnimNotify_PlaySound.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameInstance/SKGameInstance.h"
#include "Utility/SKBGMSubSystem.h"

void USKAnimNotify_PlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	
	UWorld* World = Owner->GetWorld();
	if (!World)
		return;

	USKGameInstance* SKGI = Cast<USKGameInstance>(World->GetGameInstance());
	if (!SKGI)
		return;

	if (USKBGMSubSystem* BGM = SKGI->GetSubsystem<USKBGMSubSystem>())
	{
		FVector PlayerLocation = Owner->GetActorLocation();
		BGM->PlaySoundByTag(GameplayCueTag, PlayerLocation);
	}

}
