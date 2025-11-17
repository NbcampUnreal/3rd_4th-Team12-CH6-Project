
#include "SKNativeGameplayTags.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_Team, "Team");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Player, "Team.Player");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Monster, "Team.Monster");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Monster_Boss, "Team.Monster.Boss");

UE_DEFINE_GAMEPLAY_TAG(TAG_Event, "Event");
UE_DEFINE_GAMEPLAY_TAG(TAG_Event_LeftATKTraceEnd, "Event.LeftATKTraceEnd");

UE_DEFINE_GAMEPLAY_TAG(TAG_Object, "Object");
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpikeTrap, "Object.SpikeTrap");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability, "Ability");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Dash, "Ability.Dash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Dash_Active, "Ability.Dash.Active");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Sprint, "Ability.Sprint");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interact, "Ability.Interact");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK, "Ability.LeftATK");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe, "Ability.LeftATK.Axe");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num01, "Ability.LeftATK.Axe.Num01");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num02, "Ability.LeftATK.Axe.Num02");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num03, "Ability.LeftATK.Axe.Num03");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_LeftATK_Axe_Num04, "Ability.LeftATK.Axe.Num04");



UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_RightATK, "Ability.RightATK");


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
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Axe, "Weapon.Axe");
UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Assassin, "Weapon.Assassin");


UE_DEFINE_GAMEPLAY_TAG(TAG_Sound, "Sound");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM, "Sound.BGM");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_TestMap,"Sound.BGM.TestMap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_MainTown,"Sound.BGM.MainTown");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_DungeonLevel,"Sound.BGM.DungeonLevel");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_TownLevel,"Sound.BGM.TownLevel");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_Title,"Sound.BGM.Title");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_Lobby,"Sound.BGM.Lobby");


UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_SFX, "Sound.SFX");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_SFX_ATK_00,"Sound.SFX.ATK_00");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_SFX_ATK_01,"Sound.SFX.ATK_01");


//UI Tag
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout, "UI.Layout");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_InGame, "UI.Layout.InGame");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_EscapeMenu, "UI.Layout.EscapeMenu");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Layout_Confirm, "UI.Layout.Confirm");

UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_CharacterStatus, "UI.Slot.CharacterStatus");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_QuickItem, "UI.Slot.QuickItem");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_SkillBar, "UI.Slot.SkillBar");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_GameExitRequest, "UI.Slot.GameExitRequest");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_ConfirmSlot, "UI.Slot.ConfirmSlot");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_ChracterStat, "UI.Slot.CharacterStat");



UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState,		 "PlayerState");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Idle, "PlayerState.Idle");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Move, "PlayerState.Move");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_LeftAttack,  "PlayerState.LeftAttack");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Damaged, "PlayerState.Damaged");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Groggy, "PlayerState.Groggy");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Dead, "PlayerState.Dead");


//Message Chanel Tag
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_SwitchLayout, "Message.Channel.SwitchLayout");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_RequestConfirm, "Message.Channel.RequestConfirm");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_ConfirmResponse, "Message.Channel.ConfirmResponse");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_SlotVisible, "Message.Channel.SlotVisible");