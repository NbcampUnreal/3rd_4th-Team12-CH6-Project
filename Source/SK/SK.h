// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#define UE_GET_SET_INIT(Type, Name, DefaultValue, ...) \
private: \
UPROPERTY(__VA_ARGS__) Type Name = DefaultValue; \
public: \
FORCEINLINE Type Get##Name() const { return Name; } \
FORCEINLINE void Set##Name(const Type& val) { Name = val; }

