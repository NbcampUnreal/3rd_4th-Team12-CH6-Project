// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossTriggerActor.generated.h"

/** AI Boss 활성화 트리거 델리게이트 */
DECLARE_MULTICAST_DELEGATE(FOnAITriggered);

UCLASS()
class SK_API ABossTriggerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossTriggerActor();
	
	/** BP에서 호출할 트리거 함수 */
	UFUNCTION(BlueprintCallable)
	void FireTrigger();

	/** AIController가 구독하는 델리게이트 */
	FOnAITriggered OnAITriggered;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
