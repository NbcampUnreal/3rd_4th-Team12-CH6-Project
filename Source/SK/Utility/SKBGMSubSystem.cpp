// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/SKBGMSubSystem.h"
#include "Components/AudioComponent.h"
#include "GameInstance/SKGameInstance.h"
#include "Kismet/GameplayStatics.h"

USKBGMSubSystem::USKBGMSubSystem()
{
}

void USKBGMSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// if (IsRunningListenServer())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Server BGM is not inited"));
	// 	return;
	// }

	if (UWorld* World = GetWorld())
	{
		OnPostLoadMap(World);
	}

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USKBGMSubSystem::OnPostLoadMap);
}

void USKBGMSubSystem::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	Super::Deinitialize();

	if (BGMComponent)
	{
		BGMComponent->Stop();
		BGMComponent = nullptr;
	}
	SFXSoundPool.Empty();
}


void USKBGMSubSystem::StopBGM()
{
	if (BGMComponent && BGMComponent->IsPlaying())
	{
		BGMComponent->Stop();
		UE_LOG(LogTemp, Log, TEXT("[BGM] Stopped BGM"));
	}
}

void USKBGMSubSystem::UpdateVolume()
{
	if (BGMComponent)
	{
		if (USKGameInstance* GI = Cast<USKGameInstance>(GetGameInstance()))
		{
			BGMComponent->SetVolumeMultiplier(GI->MasterVolume * GI->BGMVolume);
		}
	}
}

FGameplayTag USKBGMSubSystem::GetRecentlyMapTag()
{
	FName MapTagName = FName(*UGameplayStatics::GetCurrentLevelName(GetWorld(), true));

	FString TagName = FString::Printf(TEXT("Sound.BGM.%s"), *MapTagName.ToString());
	FGameplayTag MapTag = FGameplayTag::RequestGameplayTag(*TagName, /*bErrorIfNotFound=*/false);

	return MapTag;
}

void USKBGMSubSystem::PlayBGM_MapNow()
{
	PlayBgmByTag(GetRecentlyMapTag());
}

void USKBGMSubSystem::PlaySoundByTag(const FGameplayTag& Tag, const FVector& Location)
{
	USKGameInstance* SKGI = Cast<USKGameInstance>(GetGameInstance());
	if (!SKGI)
		return;

	const float MasterVolume = SKGI->GetMasterVolume();
	const float SFXVolume = SKGI->GetSFXVolume();
	const float FinalVolume = MasterVolume * SFXVolume;

	const TObjectPtr<USKSoundDataAsset>& SoundDataAsset = SKGI->GetSoundDataAsset();
	if (!SoundDataAsset)
		return;

	const FSKSoundData* FoundSound = SoundDataAsset->SoundList.FindByPredicate(
		[&](const FSKSoundData& Data)
		{
			return Data.SoundTag.MatchesTagExact(Tag);
		});

	if (!FoundSound || !FoundSound->Sound)
	{
		UE_LOG(LogTemp, Warning, TEXT("No sound found for tag %s"), *Tag.ToString());
		return;
	}

	UAudioComponent* AvailableComp = nullptr;
	for (UAudioComponent* Comp : SFXSoundPool)
	{
		if (IsValid(Comp) && !Comp->IsPlaying())
		{
			AvailableComp = Comp;
			break;
		}
	}

	// 🔹 없으면 새 컴포넌트 생성
	if (!AvailableComp)
	{
		AvailableComp = NewObject<UAudioComponent>(GetWorld());
		if (AvailableComp)
		{
			AvailableComp->bAutoActivate = false;
			AvailableComp->RegisterComponentWithWorld(GetWorld());
			SFXSoundPool.Add(AvailableComp);
		}
	}

	// 🔹 사운드 세팅 및 재생
	if (AvailableComp)
	{
		AvailableComp->SetWorldLocation(Location);
		AvailableComp->SetSound(FoundSound->Sound);

		AvailableComp->SetVolumeMultiplier(FinalVolume);

		AvailableComp->Play();
	}
}

void USKBGMSubSystem::PlayUISoundByTag(const FGameplayTag& Tag)
{
	USKGameInstance* SKGI = Cast<USKGameInstance>(GetGameInstance());
	if (!SKGI)
		return;

	const float MasterVolume = SKGI->GetMasterVolume();
	const float SFXVolume = SKGI->GetSFXVolume();
	const float FinalVolume = MasterVolume * SFXVolume;

	const TObjectPtr<USKSoundDataAsset>& SoundDataAsset = SKGI->GetSoundDataAsset();
	if (!SoundDataAsset)
		return;

	const FSKSoundData* FoundSound = SoundDataAsset->SoundList.FindByPredicate(
		[&](const FSKSoundData& Data)
		{
			return Data.SoundTag.MatchesTagExact(Tag);
		});

	if (!FoundSound || !FoundSound->Sound)
	{
		UE_LOG(LogTemp, Warning, TEXT("No sound found for tag %s"), *Tag.ToString());
		return;
	}

	UAudioComponent* AvailableComp = nullptr;
	for (UAudioComponent* Comp : SFXSoundPool)
	{
		if (IsValid(Comp) && !Comp->IsPlaying())
		{
			AvailableComp = Comp;
			break;
		}
	}

	// 🔹 없으면 새 컴포넌트 생성
	if (!AvailableComp)
	{
		AvailableComp = NewObject<UAudioComponent>(GetWorld());
		if (AvailableComp)
		{
			AvailableComp->bAutoActivate = false;
			AvailableComp->RegisterComponentWithWorld(GetWorld());
			SFXSoundPool.Add(AvailableComp);
		}
	}

	// 🔹 사운드 세팅 및 재생
	if (AvailableComp)
	{
		AvailableComp->Stop();

		AvailableComp->SetSound(FoundSound->Sound);

		AvailableComp->bAllowSpatialization = false;
		AvailableComp->SetUISound(true);

		AvailableComp->AttenuationSettings = nullptr;

		AvailableComp->SetVolumeMultiplier(FinalVolume);

		AvailableComp->Play();
	}
}

void USKBGMSubSystem::PlayBgmByTag(const FGameplayTag& Tag)
{
	if (CurrentBGMTag.IsValid() && CurrentBGMTag.MatchesTagExact(Tag))
	{
		UE_LOG(LogTemp, Verbose, TEXT("BGM '%s' already playing, skip"), *Tag.ToString());
		return;
	}


	UGameInstance* GI = GetGameInstance();
	USKGameInstance* SKGameInstance = Cast<USKGameInstance>(GI);

	if (!SKGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid GameInstance in SoundSubsystem"));
		return;
	}

	const TObjectPtr<USKSoundDataAsset>& SoundDataAsset = SKGameInstance->GetSoundDataAsset();
	if (!SoundDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoundDataAsset is not set in GameInstance"));
		return;
	}

	const FSKSoundData* FoundSound = SoundDataAsset->SoundList.FindByPredicate(
		[&](const FSKSoundData& Data)
		{
			return Data.SoundTag.MatchesTagExact(Tag);
		});

	if (!FoundSound)
	{
		UE_LOG(LogTemp, Warning, TEXT(" SoundTag '%s' not found in SoundDataAsset"), *Tag.ToString());
		return;
	}
	if (!FoundSound->Sound)
	{
		UE_LOG(LogTemp, Warning, TEXT(" Sound asset not assigned for tag: %s"), *Tag.ToString());
		return;
	}
	if (!FoundSound->Sound->IsValidLowLevelFast())
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound asset is not fully loaded!"));
		return;
	}

	// 기존 BGM 중지
	if (BGMComponent && BGMComponent->IsPlaying())
	{
		BGMComponent->Stop();
	}
	CurrentBGMTag = Tag;

	// 새 BGM 재생
	float FinalVolume = SKGameInstance->MasterVolume * SKGameInstance->BGMVolume;

	GetWorld()->GetTimerManager().SetTimerForNextTick([this, FoundSound, FinalVolume]()
	{
		BGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), FoundSound->Sound, FinalVolume, 1.0f, 0.0f, nullptr,
		                                              false);
		if (BGMComponent)
		{
			// BGMComponent->SetLoop(FoundSound->bLoop);
			BGMComponent->bIsUISound = true;
			BGMComponent->Play();
		}
	});
}


bool USKBGMSubSystem::IsRunningListenServer() const
{
	UWorld* World = GetWorld();
	return (World && World->GetNetMode() == NM_ListenServer);
}

void USKBGMSubSystem::OnPostLoadMap(UWorld* LoadedWorld)
{
	if (!LoadedWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BGM] LoadedWorld is invalid"));
		return;
	}

	// 현재 맵 이름 확인
	// FName MapTagName = FName(*UGameplayStatics::GetCurrentLevelName(LoadedWorld, true));
	// FString TagName = FString::Printf(TEXT("Sound.BGM.%s"), *MapTagName.ToString());
	// FGameplayTag Tag = FGameplayTag::RequestGameplayTag(*TagName, /*bErrorIfNotFound=*/false);
	FGameplayTag Tag = GetRecentlyMapTag();
	if (!Tag.IsValid())
	{
		// UE_LOG(LogTemp, Warning, TEXT("GameplayTag not found: %s"), *TagName);
		StopBGM();
		return;
	}


	PlayBgmByTag(Tag);
}
