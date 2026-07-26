// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru


#include "Abilities/TowerDefenceTags.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Turret_State_Ready, "Turret.State.Ready", "Tag when turret is ready for shot");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Turret_State_NeedsCooldown, "Turret.State.NeedsCooldown", "Tag when turret needs to cooling down between shots");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Turret_State_Cooldown, "Turret.State.Cooldown", "Tag when turret is cooling down between shots");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Turret_State_Empty, "Turret.State.Empty", "Tag when turret is out of ammo");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Turret_State_Reloading, "Turret.State.Reloading", "Tag when turret is reloading");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Damage_Take, "Action.Damage.Take", "Tag for taking damage");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Damage_Heal, "Action.Damage.Heal", "Tag for healing damage");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Money_Receive, "Action.Money.Receive", "Tag for receiving money");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Money_Spend, "Action.Money.Spend", "Tag for spending money");
