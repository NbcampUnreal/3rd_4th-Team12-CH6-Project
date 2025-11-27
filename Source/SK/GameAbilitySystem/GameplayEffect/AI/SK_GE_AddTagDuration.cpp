#include "GameAbilitySystem/GameplayEffect/AI/SK_GE_AddTagDuration.h"

USK_GE_AddTagDuration::USK_GE_AddTagDuration()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(3.0f));
	
}
