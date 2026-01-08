// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "SKBGMSubSystem.generated.h"

//struct FGameplayTag;
/**
 * 
 */
UCLASS()
class SK_API USKBGMSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USKBGMSubSystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "SK|Sound" ,meta = (DisplayName = "PlaySoundByTag"))
	void PlaySoundByTag(const FGameplayTag& Tag, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "SK|Sound" ,meta = (DisplayName = "PlayUISoundByTag"))
	void PlayUISoundByTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, Category = "SK|Sound" ,meta = (DisplayName = "PlayBGMByTag"))
	void PlayBgmByTag(const FGameplayTag& Tag);
	void StopBGM();
	void UpdateVolume();

	FGameplayTag GetRecentlyMapTag();
	void PlayBGM_MapNow();
private:
	bool IsRunningListenServer() const;
	void OnPostLoadMap(UWorld* LoadedWorld); // 레벨이 로드 될 때마다 해당 함수 호출(Bind)

	UPROPERTY()
	TObjectPtr<UAudioComponent> BGMComponent;

	UPROPERTY()
	TArray<TObjectPtr<UAudioComponent>> SFXSoundPool;

	// 최대 풀 크기
	UPROPERTY(EditAnywhere, Category="Sound Pool")
	int32 MaxPoolSize = 80;


	FGameplayTag CurrentBGMTag;
};
