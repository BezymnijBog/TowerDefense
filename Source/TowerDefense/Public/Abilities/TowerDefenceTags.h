// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Turret_State_Ready);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Turret_State_NeedsCooldown);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Turret_State_Cooldown);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Turret_State_Empty);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Turret_State_Reloading);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Damage_Take);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Damage_Heal);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Money_Receive);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Money_Spend);
