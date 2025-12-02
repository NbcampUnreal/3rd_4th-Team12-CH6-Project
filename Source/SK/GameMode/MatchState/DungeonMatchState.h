// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonMatchState.generated.h"


UENUM(BlueprintType)
enum class EDungeonMatchState : uint8
{
	None,
	Dungeon_Loading,	//맵 로딩
	Dungeon_PlayerReady,//플레이어 준비 완료
	Dungeon_InProgress,	//전투 시작
	Dungeon_Cleared,	//클리어
	Dungeon_Failed		//실패
};