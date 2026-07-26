// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/BuildMenu.h"

#include "TowerDefense.h"
#include "Widgets/BuildMenuElement.h"

void UBuildMenu::UpdateBuildingAbility(float NewMoney) const
{
    for (UBuildMenuElement* const Child : GetElements())
    {
        Child->UpdateState(NewMoney);
    }
}

bool UBuildMenu::Initialize()
{
    bool bResult = Super::Initialize();
    UE_LOG(LogTowerDefense, Display, TEXT("Initialize Turret cost: %d"), AttackingTurretInfo.BuildCost);
    return bResult;
}

void UBuildMenu::NativePreConstruct()
{
    UE_LOG(LogTowerDefense, Display, TEXT("NativePreConstruct Turret cost: %d"), AttackingTurretInfo.BuildCost);
    Super::NativePreConstruct();
}

void UBuildMenu::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTowerDefense, Display, TEXT("NativeConstruct Turret cost: %d"), AttackingTurretInfo.BuildCost);
    AttackingTurret->SetElementInfo(AttackingTurretInfo);
    BuffingTurret->SetElementInfo(BuffingTurretInfo);
    DebuffingTurret->SetElementInfo(DebuffingTurretInfo);
}

TArray<UBuildMenuElement*> UBuildMenu::GetElements() const
{
    return { AttackingTurret, BuffingTurret, DebuffingTurret };
}
