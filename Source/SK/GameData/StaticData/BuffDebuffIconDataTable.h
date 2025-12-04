#pragma once

#include "CoreMinimal.h"
#include "StaticDataBase.h"
#include "BuffDebuffIconDataTable.generated.h"

USTRUCT(BlueprintType)
struct FBuffDebuffIconDataTable : public FStaticDataBase
{
	GENERATED_BODY()
public:
	/** 메인 아이콘 (체력/공격력/방어력/속도 등) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> StatIcon;

	/** 증가/감소 아이콘 (▲, ▼ 같은 효과 아이콘) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> ModifierIcon;
};
