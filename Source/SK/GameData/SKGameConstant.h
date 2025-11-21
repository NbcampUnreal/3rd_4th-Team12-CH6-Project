// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//상수 정의용 클래스
namespace SKConstant
{
	constexpr float MaxArmorValue = 1000.f;
	constexpr float ArmorDamageDeclineRate = 100.f;
	constexpr int DamageSteps = 6;
	constexpr int LeftMaxCombo_Axe = 4;
	constexpr int RightMaxCombo_Axe = 4;
	constexpr int32 GA_Interact_ID = 3;

	constexpr ECollisionChannel ECC_Interactable = ECC_GameTraceChannel1;
}

//constexpr = 런타임(프로그램실행중)이 아닌 컴파일(프로그램빌드) 중에 계산됨 