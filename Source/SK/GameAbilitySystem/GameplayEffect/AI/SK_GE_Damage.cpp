#include "GameAbilitySystem/GameplayEffect/AI/SK_GE_Damage.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"

USK_GE_Damage::USK_GE_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo DamageModifier;
	DamageModifier.Attribute = USKAttributeSet::GetHealthAttribute();
	DamageModifier.ModifierOp = EGameplayModOp::Additive;

	//FSetByCallerFloat SetByCallerData;
	//SetByCallerData.DataTag = GASProjectTags::SetByCaller_Damage;
	//FGameplayEffectModifierMagnitude ModMagnitude(SetByCallerData);
	FGameplayEffectModifierMagnitude ModMagnitude(-10.0f);
	DamageModifier.ModifierMagnitude = ModMagnitude;

	Modifiers.Add(DamageModifier);
}
