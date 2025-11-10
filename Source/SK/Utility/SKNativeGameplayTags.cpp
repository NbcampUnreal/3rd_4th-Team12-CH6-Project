
#include "SKNativeGameplayTags.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_Team, "Team");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Player, "Team.Player");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Monster, "Team.Monster");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Monster_Boss, "Team.Monster.Boss");


UE_DEFINE_GAMEPLAY_TAG(TAG_Object, "Object");
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpikeTrap, "Object.SpikeTrap");

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability, "Ability");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Dash, "Ability.Dash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Dash_Active, "Ability.Dash.Active");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Sprint, "Ability.Sprint");


UE_DEFINE_GAMEPLAY_TAG(TAG_CoolDown, "Cooldown");
UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Ability, "Cooldown.Ability");
UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Ability_Dash, "Cooldown.Ability.Dash");



UE_DEFINE_GAMEPLAY_TAG(TAG_Sound, "Sound");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM, "Sound.BGM");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_TestMap,"Sound.BGM.TestMap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_BGM_MainTown,"Sound.BGM.MainTown");
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
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_GameExitRequest, "UI.Slot.GameExitRequest");
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Slot_ConfirmSlot, "UI.Slot.ConfirmSlot");



UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState,		 "PlayerState");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Idle, "PlayerState.Idle");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Move, "PlayerState.Move");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Attack,  "PlayerState.Attack");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Damaged, "PlayerState.Damaged");
UE_DEFINE_GAMEPLAY_TAG(TAG_PlayerState_Groggy, "PlayerState.Groggy");


//Message Chanel Tag
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_SwitchLayout, "Message.Channel.SwitchLayout");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_RequestConfirm, "Message.Channel.RequestConfirm");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_ConfirmResponse, "Message.Channel.ConfirmResponse");
UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Channel_SlotVisible, "Message.Channel.SlotVisible");