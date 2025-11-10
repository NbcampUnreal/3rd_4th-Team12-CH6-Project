// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticDataBase.h"
#include "EquipmentDataTable.generated.h"

/**
 * 장비 타입 열거형
 * - 데이터 테이블과 블루프린트에서 선택형 드롭다운으로 표시됨
 */
UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Weapon		UMETA(DisplayName = "Weapon"),      // 무기
	Armor		UMETA(DisplayName = "Armor"),       // 방어구
	Top			UMETA(DisplayName = "Top"),         // 상의
	Bottom		UMETA(DisplayName = "Bottom"),      // 하의
	Accessory	UMETA(DisplayName = "Accessory"),   // 장신구
};

/**
 * 장비 데이터 테이블 구조
 * - 무기, 방어구, 반지 등 구분 포함
 */
USTRUCT(BlueprintType)
struct FEquipmentData : public FStaticDataBase
{
	GENERATED_BODY()

public:
	/** 장비 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment")
	FName EquipmentName;

	/** 장비 타입 (무기, 상의, 하의, 반지 등) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment")
	EEquipmentType EquipmentType = EEquipmentType::Weapon;

	/** 장비 액터 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment")
	TSoftClassPtr<AActor> EquipmentClass;

	/** 장비 공격력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment")
	int32 Attack = 0;

	/** 장비 방어력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment")
	int32 Defense = 0;

	/** UI용 아이콘 이미지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment")
	TSoftObjectPtr<UTexture2D> Icon;
};