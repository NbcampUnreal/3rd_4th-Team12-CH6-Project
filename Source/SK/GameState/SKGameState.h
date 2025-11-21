// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameState.h"
#include "GameData/SKGameRuleData.h"
#include "SKGameState.generated.h"

/**
 * 
 */
UCLASS()
class SK_API ASKGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//메시지 전달 형식 확인 용 함수
	UFUNCTION(BlueprintCallable)
	void TestMessage();
	//메시지 전달 테스트 멀티캐스트
	UFUNCTION(NetMulticast, Reliable)
	void MulticastTestMessage();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess));
	USKGameRuleData* GameRuleData;
	
	
};
