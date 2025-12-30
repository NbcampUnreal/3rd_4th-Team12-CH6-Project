// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SK_API SKGameConstants
{
public:
	SKGameConstants();
	~SKGameConstants();

	//레벨 경로
	static constexpr const TCHAR* TownLevel = TEXT("TownLevel");
	static constexpr const TCHAR* Tutorial = TEXT("RuinsTutorial");

	inline static const TMap<int32, FString> DungeonLevels = {
		{ 1, TEXT("MVP") },
		{ 2, TEXT("RuinsRenuewal") },
	};
};
