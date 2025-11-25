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

UE_DEFINE_GAMEPLAY_TAG(TAG_Object, "Object");
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpikeTrap, "Object.SpikeTrap");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability, "Ability");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Dash, "Ability.Dash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Dash_Active, "Ability.Dash.Active");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Sprint, "Ability.Sprint");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interact, "Ability.Interact");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_OpenChest, "Ability.OpenChest");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK, "Ability.LeftATK");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe, "Ability.LeftATK.Axe");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num01, "Ability.LeftATK.Axe.Num01");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num02, "Ability.LeftATK.Axe.Num02");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num03, "Ability.LeftATK.Axe.Num03");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num04, "Ability.LeftATK.Axe.Num04");


UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_RightATK, "Ability.RightATK");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Melee, "Ability.Melee");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Die, "Ability.Die");


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

UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon, "Weapon");
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Axe, "Weapon.Axe"); // Axe-> TwoHanded
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Assassin, "Weapon.Assassin");
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

UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_CharacterStatus, "UI.Slot.CharacterStatus");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_QuickItem, "UI.Slot.QuickItem");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_SkillBar, "UI.Slot.SkillBar");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_GameExitRequest, "UI.Slot.GameExitRequest");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_ConfirmSlot, "UI.Slot.ConfirmSlot");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_ChracterStat, "UI.Slot.CharacterStat");
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
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Movement_Dash, "State.Movement.Dash");
// State-Action (행동)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action, "State.Action");
// State-Action-ATK (공격)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_ATK, "State.Action.ATK");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_ATK_Skill, "State.Action.ATK.Skill");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_ATK_LeftMelee, "State.Action.ATK.LeftMelee");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_ATK_RigthMelee, "State.Action.ATK.RigthMelee");

UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_Melee, "State.Action.Melee");
// State-Action-Interaction (상호작용)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Action_Interaction, "State.Action.Interaction");
// State-Condition (실제 플레이에 영향끼치는 상태)
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition, "State.Condition");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_Stun, "State.Condition.Stun");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_Groggy, "State.Condition.Groggy");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_Death, "State.Condition.Death");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Condition_Hit, "State.Condition.Hit");

//Message Chanel Tag
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_SwitchLayout, "Message.Channel.SwitchLayout");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_RequestConfirm, "Message.Channel.RequestConfirm");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_ConfirmResponse, "Message.Channel.ConfirmResponse");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_SlotVisible, "Message.Channel.SlotVisible");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_ToolTipItem, "Message.Channel.ToolTipItem");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_ItemSwitchSelect, "Message.Channel.ItemSwitchSelect");
