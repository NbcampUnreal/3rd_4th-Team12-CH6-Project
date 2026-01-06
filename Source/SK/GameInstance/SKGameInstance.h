// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameData/SKSoundDataAsset.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "SKGameInstance.generated.h"

class UCommonActivatableWidget;
struct FLoadingUIVisible;
class UPrimaryDataAsset;
/**
 * 
 */
UCLASS()
class SK_API USKGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	// virtual void Shutdown() override;
	//
	// void HandleLevelChanged(const FName& LevelName);

#pragma region Multi_Single_LevelTransition
	//호스트로 마을 세션 열기
	UFUNCTION(BlueprintCallable)
	void HostTownSession();

	//클라이언트로 호스트 세션 참가
	UFUNCTION(BlueprintCallable)
	void JoinHostTown(const FString& Address);

	//던전 / 마을 전환 (서버 전용)
	void TravelToDungeon(int32 DungeonID);
	void TravelToTown();

	//세션 종료 후 로컬 마을 복귀
	UFUNCTION(BlueprintCallable)
	void LeaveSession();
	
#pragma endregion Multi_Single_LevelTransition

#pragma region SOUND
	
	const TObjectPtr<USKSoundDataAsset>& GetSoundDataAsset();
	
	UFUNCTION(BlueprintCallable, Category = "SK|Sound|Volume")
	void SetMasterVolume(float InVolume);
	float GetMasterVolume() const { return MasterVolume; }
	
	UFUNCTION(BlueprintCallable, Category = "SK|Sound|Volume")
	void SetBGMVolume(float InVolume);
	float GetBGMVolume() const { return BGMVolume; }
	
	UFUNCTION(BlueprintCallable, Category = "SK|Sound|Volume")
	void SetSFXVolume(float InVolume);
	float GetSFXVolume() const { return SFXVolume; }
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SK|Audio")
	TObjectPtr<USKSoundDataAsset> SoundDataAsset;

	//종합 크기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SK|Audio|Volume")
	float MasterVolume = 0.5f;

	//브금 크기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SK|Audio|Volume")
	float BGMVolume = 0.5f;

	//UI 사운드 & 상호작용,이펙트 크기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SK|Audio|Volume")
	float SFXVolume = 0.5f;

#pragma endregion

#pragma region Loading_UI

	// 로딩 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SK|UI")
	TSubclassOf<UUserWidget> LoadingWidgetClass;

	// 생성된 로딩 위젯 인스턴스
	UPROPERTY()
	UUserWidget* LoadingWidgetInstance = nullptr;
	
	TSharedPtr<SWidget> LoadingSlateWidget;
	
	// 로딩 화면 표시/숨김
	UFUNCTION(BlueprintCallable, Category = "SK|UI")
	void ShowLoadingScreen(bool bShow);
	
	FSKGameplayMessageListenerHandle LoadingUIVisibleHandle;

	void OnLoadingUIVisibleMessageReceived(FGameplayTag Channel, const FLoadingUIVisible& Message);
#pragma endregion Loading_UI
	
	//임시. 추후 데이터 에셋 매니저 나오면 변경
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPrimaryDataAsset> TestLayoutData;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPrimaryDataAsset> TestConfirmData;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCommonActivatableWidget> TitleWidgetClass;

	UPROPERTY()
	TObjectPtr<UCommonActivatableWidget> TitleWidgetInstance;

	UFUNCTION(BlueprintCallable)
	void ShowTitleUI();

	
	void StartGameFromTitle();
};
