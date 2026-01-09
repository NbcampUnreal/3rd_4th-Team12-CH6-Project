#include "GameAbilitySystem/GameplayEffect/AI/SK_GE_AI_Stamina.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"
#include "Utility/SKNativeGameplayTags.h"

USK_GE_AI_Stamina::USK_GE_AI_Stamina()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo StaminaModifier;
	StaminaModifier.Attribute = USKAIAttributeSet::GetStaminaAttribute();
	StaminaModifier.ModifierOp = EGameplayModOp::Override;

	FSetByCallerFloat SetByCallerData;
	SetByCallerData.DataTag = TAG_Data_Stamina;

	StaminaModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerData);

	Modifiers.Add(StaminaModifier);
}
