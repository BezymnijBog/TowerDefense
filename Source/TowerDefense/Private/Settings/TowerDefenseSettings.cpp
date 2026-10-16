// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Settings/TowerDefenseSettings.h"

int32 UTowerDefenseSettings::GetReward(EDayEventType EventType) const
{
    const int32* Reward = MoneyReward.Find(EventType);
    return Reward ? *Reward : 0;
}

FName UTowerDefenseSettings::GetCategoryName() const
{
    return NAME_Game;
}

FName UTowerDefenseSettings::GetSectionName() const
{
    return TEXT("TowerDefence");
}

FText UTowerDefenseSettings::GetSectionDescription() const
{
    return FText::FromString(TEXT("Section for the gameplay configurations"));
}

const UTowerDefenseSettings& UTowerDefenseSettings::Get()
{
    return *GetDefault<ThisClass>();
}
