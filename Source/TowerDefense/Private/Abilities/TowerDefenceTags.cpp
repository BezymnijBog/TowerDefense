// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/TowerDefenceTags.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Attack_Cooldown, TEXT("Action.Attack.Cooldown"), "Tag for attack cooldown");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Attack_Progress, TEXT("Action.Attack.Progress"), "Tag for attack in progress");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Attack_Make, TEXT("Action.Attack.Make"), "Tag for making attack");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Projectile_Launch, TEXT("Action.Projectile.Launch"), "Tag for launching projectile");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Damage_Calculate, TEXT("Action.Damage.Calculate"), "Tag for damage calculation");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Damage_Deal, TEXT("Action.Damage.Take"), "Tag for dealing damage");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Damage_Take, TEXT("Action.Damage.Take"), "Tag for taking damage");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Damage_Heal, TEXT("Action.Damage.Heal"), "Tag for healing damage");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Money_Change, TEXT("Action.Money.Change"), "Tag for changing the amount of money");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Wave_Start, TEXT("Action.Wave.Start"), "Tag for starting the wave");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Wave_Finished, TEXT("Action.Wave.Finished"), "Tag for finishing the wave");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Modifier_Damage_Heavy, TEXT("Modifier.Damage.Heavy"), "Tag for heavy damage from machines");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Modifier_Damage_TwoHanded, TEXT("Modifier.Damage.TwoHanded"), "Tag two handed attack from units");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mode_Placement_Start, TEXT("Mode.Placement.Start"), "Tag for placement mode start");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mode_Placement_End, TEXT("Mode.Placement.End"), "Tag for placement mode end");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_State_Dead, TEXT("Character.State.Dead"), "Tag for dead character");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Building_Construct, TEXT("Action.Building.Construct"), "Request construction or upgrade of a target");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Damage_Thrust, TEXT("Weapon.Damage.Thrust"), "Tag for thrusting attacks (otherwise it is swinging)");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_DamageType_Cr, TEXT("Weapon.DamageType.Cr"), "Tag for crushing damage type");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_DamageType_Cut, TEXT("Weapon.DamageType.Cut"), "Tag for cutting damage type");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_DamageType_Imp, TEXT("Weapon.DamageType.Imp"), "Tag for impaling damage type");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Parry_No, TEXT("Weapon.Parry.No"), "Tag for weapon with no parry");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Parry_Unbalanced, TEXT("Weapon.Parry.Unbalanced"), "Tag for unbalanced weapon");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Projectile, TEXT("Weapon.Pojectile"), "Tag for projectile attacks");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Shield, TEXT("Weapon.Shield"), "Tag for shield defense against attack");
