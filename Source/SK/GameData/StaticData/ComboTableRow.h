#pragma once

#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "ComboTableRow.generated.h"

USTRUCT(BlueprintType)
struct FComboTableRow : public FTableRowBase
{
	GENERATED_BODY()

	// 현재 콤보 상태 (None, L, LR, LRL...)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag FromState;

	// 입력 태그 (Input.Attack.Left / Right)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag;

	// 다음 콤보 상태
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ToState;
};
