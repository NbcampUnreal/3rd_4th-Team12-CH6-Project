#include "SKNativeGameplayTags.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_Team, "Team");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Player, "Team.Player");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Monster, "Team.Monster");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Monster_Boss, "Team.Monster.Boss");

UE_DEFINE_GAMEPLAY_TAG(TAG_Event, "Event");
UE_DEFINE_GAMEPLAY_TAG(TAG_Event_LeftATKTraceEnd, "Event.LeftATKTraceEnd");
UE_DEFINE_GAMEPLAY_TAG(TAG_Event_EndAbility, "Event.EndAbility");
UE_DEFINE_GAMEPLAY_TAG(TAG_Event_Hit, "Event.Hit");
UE_DEFINE_GAMEPLAY_TAG(TAG_Event_Death, "Event.Death");
UE_DEFINE_GAMEPLAY_TAG(TAG_Event_MoveComplete, "Event.MoveComplete");
UE_DEFINE_GAMEPLAY_TAG(TAG_Event_AnimNotify, "Event.AnimNotify");

UE_DEFINE_GAMEPLAY_TAG(TAG_Object, "Object");
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpikeTrap, "Object.SpikeTrap");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability, "Ability");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Dash, "Ability.Dash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Dash_Active, "Ability.Dash.Active");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Sprint, "Ability.Sprint");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interaction, "Ability.InteractionTrace");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_SimpleInteract, "Ability.SimpleInteract");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK, "Ability.LeftATK");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe, "Ability.LeftATK.Axe");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_TwoHanded, "Ability.LeftATK.TwoHanded");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Cancel, "Ability.LeftATK.Cancel");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num01, "Ability.LeftATK.Axe.Num01");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num02, "Ability.LeftATK.Axe.Num02");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num03, "Ability.LeftATK.Axe.Num03");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num04, "Ability.LeftATK.Axe.Num04");


UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_RightATK, "Ability.RightATK");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Guard, "Ability.Guard");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Skill_01, "Ability.Skill.01");


UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Melee, "Ability.Melee");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Die, "Ability.Die");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Cancel, "Ability.Wander");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Chase, "Ability.Chase");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Ready, "Ability.Ready");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Rush, "Ability.Rush");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_JumpRush, "Ability.JumpRush");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Recovery, "Ability.Recovery");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Backstep, "Ability.Backstep");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_ProjectileAttack, "Ability.ProjectileAttack");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Item_HealPotion, "Ability.Item.HealPotion");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Item_AttackPotion, "Ability.Item.AttackPotion");

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_LeftATK);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_LeftATK_Axe);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_LeftATK_Axe_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_LeftATK_Axe_2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_LeftATK_Axe_3);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_LeftATK_Axe_4);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_RightATK);

UE_DEFINE_GAMEPLAY_TAG(TAG_CoolDown, "Cooldown");
UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Ability, "Cooldown.Ability");
UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Ability_Dash, "Cooldown.Ability.Dash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Item_HealPotion, "Cooldown.Item.HealPotion");
UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Item_AttackPotion, "Cooldown.Item.AttackPotion");


UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon, "Weapon");
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Axe, "Weapon.Axe"); // Axe-> TwoHanded
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Assassin, "Weapon.Assassin");
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_GreatSword, "Weapon.GreatSword");
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_TwoHanded, "Weapon.TwoHanded");
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Dual, "Weapon.Dual");
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Katana, "Weapon.Katana");
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Spear, "Weapon.Spear");
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Warrior, "Weapon.Warrior");


UE_DEFINE_GAMEPLAY_TAG(TAG_Sound, "Sound");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM, "Sound.BGM");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_TestMap, "Sound.BGM.TestMap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_MainTown, "Sound.BGM.MainTown");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_DungeonLevel, "Sound.BGM.DungeonLevel");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_TownLevel, "Sound.BGM.TownLevel");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_MVP, "Sound.BGM.MVP");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_Boss_00, "Sound.BGM.Boss.00");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_Boss_01, "Sound.BGM.Boss.01");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_Title, "Sound.BGM.Title");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_Lobby, "Sound.BGM.Lobby");


UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_SFX, "Sound.SFX");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_SFX_ATK_00, "Sound.SFX.ATK_00");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_SFX_ATK_01, "Sound.SFX.ATK_01");


//UI Tag
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout, "UI.Layout");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_InGame, "UI.Layout.InGame");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_EscapeMenu, "UI.Layout.EscapeMenu");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_Confirm, "UI.Layout.Confirm");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_Inventory, "UI.Layout.Inventory");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_EquipmentMain, "UI.Layout.EquipmentMain");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_EquipmentSelect, "UI.Layout.EquipmentSelect");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_QuickSlotSelect, "UI.Layout.QuickSlotSelect");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_DungeonSelect, "UI.Layout.DungeonSelect");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_BonfireMenu, "UI.Layout.BonfireMenu");

UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_CharacterStatus, "UI.Slot.CharacterStatus");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_QuickItem, "UI.Slot.QuickItem");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_SkillBar, "UI.Slot.SkillBar");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_GameExitRequest, "UI.Slot.GameExitRequest");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_ConfirmSlot, "UI.Slot.ConfirmSlot");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_ChracterStat, "UI.Slot.CharacterStat");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_BossHP, "UI.Slot.BossHP");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_BossClear, "UI.Slot.BossClear");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_DungeonStart, "UI.Slot.DungeonStart");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_CharacterDeath, "UI.Slot.CharacterDeath");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_BuffDebuff, "UI.Slot.BuffDebuff");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_GoldStat, "UI.Slot.GoldStat");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_ItemAddInfo, "UI.Slot.ItemAddInfo");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_InventoryList, "UI.Slot.InventoryList");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_InventoryItemToolTip, "UI.Slot.InventoryItemToolTip");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_EquipMainList, "UI.Slot.EquipMainList");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_EquipMainToolTip, "UI.Slot.EquipMainToolTip");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_EquipMainStat, "UI.Slot.EquipMainStat");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_EquipList, "UI.Slot.EquipList");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_EquipToolTip, "UI.Slot.EquipToolTip");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_QuickSlotList, "UI.Slot.QuickSlotList");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_QuickSlotToolTip, "UI.Slot.QuickSlotToolTip");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_Interaction, "UI.Slot.Interaction");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_DungeonSelect, "UI.Slot.DungeonSelect");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_BonfireMenu, "UI.Slot.BonfireMenu");


UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState, "PlayerState");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Idle, "PlayerState.Idle");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Move, "PlayerState.Move");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_ATK, "PlayerState.ATK");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_LeftAttack, "PlayerState.LeftAttack");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Damaged, "PlayerState.Damaged");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Groggy, "PlayerState.Groggy");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Dead, "PlayerState.Dead");


// Root 태그
UE_DEFINE_GAMEPLAY_TAG(TAG_State, "State");

// State-Posture (자세/위치)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Posture, "State.Posture");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Posture_Grounded, "State.Posture.Grounded");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Posture_Air, "State.Posture.Air");
// State-Movement (이동)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Movement, "State.Movement");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Movement_Idle, "State.Movement.Idle");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Movement_Walk, "State.Movement.Walk");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Movement_Sprint, "State.Movement.Sprint");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Movement_Dash, "State.Movement.Dash");
// State-Action (행동)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action, "State.Action");
// State-Action-ATK (공격)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_ATK, "State.Action.ATK");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_ATK_Skill, "State.Action.ATK.Skill");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_ATK_LeftMelee, "State.Action.ATK.LeftMelee");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_ATK_RightMelee, "State.Action.ATK.RightMelee");

UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_Melee, "State.Action.Melee");
// State-Action-Interaction (상호작용)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_Interaction, "State.Action.Interaction");

// 회피
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_Dodge_Evade, "State.Action.Dodge.Evade");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_Dodge_Step, "State.Action.Dodge.Step");

// State-Condition (실제 플레이에 영향끼치는 상태)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition, "State.Condition");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_Stun, "State.Condition.Stun");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_Groggy, "State.Condition.Groggy");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_Death, "State.Condition.Death");

// 히트, 넉백, 넉다운
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_Hit, "State.Condition.Hit");

//AI 행동 제어
UE_DEFINE_GAMEPLAY_TAG(TAG_AI, "AI");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Perception, "AI.Perception");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Combat, "AI.Combat");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Ready, "AI.Ready");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Death, "AI.Death");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Melee, "AI.Melee");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Rush, "AI.Rush");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_JumpRush, "AI.JumpRush");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Backstep, "AI.Backstep");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Recovery, "AI.Recovery");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_ProjectileAttack, "AI.ProjectileAttack");


UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_KnockBack, "State.Condition.KnockBack");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_KnockDown, "State.Condition.KnockDown");

//Message Chanel Tag
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_SwitchLayout, "Message.Channel.SwitchLayout");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_RequestConfirm, "Message.Channel.RequestConfirm");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_ConfirmResponse, "Message.Channel.ConfirmResponse");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_SlotVisible, "Message.Channel.SlotVisible");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_ToolTipItem, "Message.Channel.ToolTipItem");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_ItemSwitchSelect, "Message.Channel.ItemSwitchSelect");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_ItemAddInfo, "Message.Channel.ItemAddInfo");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_UseQuickSlotItem, "Message.Channel.UseQuickSlotItem");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_BossWidgetSetting, "Message.Channel.BossWidgetSetting");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_LoadingUIVisible, "Message.Channel.LoadingUIVisible");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_UIInteraction, "Message.Channel.UIInteraction");

UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Health, "Item.Health");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Attack, "Item.Attack");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Armor, "Item.Armor");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Stamina, "Item.Stamina");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Gold, "Item.Gold");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Duration, "Item.Duration");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Cooldown, "Item.Cooldown");

UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Item_HealPotion, "GameplayCue.Item.HealPotion");
UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Item_AttackPotion, "GameplayCue.Item.AttackPotion");
