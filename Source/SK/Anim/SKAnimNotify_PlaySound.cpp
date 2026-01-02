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

	UAbilitySystemComponent* ASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

	if (!ASC) return;

	UWorld* World = Owner->GetWorld();
	if (!World)
		return;

	USKGameInstance* SKGI = Cast<USKGameInstance>(World->GetGameInstance());
	if (!SKGI)
		return;

	const float SFXVolume = SKGI->GetSFXVolume();
	const float MasterVolume = SKGI->GetMasterVolume();
	float FinalVolume = MasterVolume * SFXVolume;


	if (USKBGMSubSystem* BGM = SKGI->GetSubsystem<USKBGMSubSystem>())
	{
		FVector PlayerLocation = Owner->GetActorLocation();
		BGM->PlaySoundByTag(GameplayCueTag, PlayerLocation);
	}
	
	// FGameplayCueParameters CueParams;
	// CueParams.RawMagnitude = FinalVolume; 
	//
	// FGameplayEventData EventData;
	// EventData.EventTag = GameplayCueTag;
	// EventData.Instigator = Owner;
	// EventData.Target = Owner;
	//
	//
	// ASC->ExecuteGameplayCue(GameplayCueTag, CueParams);
}
