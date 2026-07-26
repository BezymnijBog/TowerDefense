// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/BuildMenuElement.h"

#include "Components/Button.h"
#include "Player/TowerDefensePlayerController.h"

float UBuildMenuElement::GetCost() const
{
    return BuildingInfo.BuildCost;
}

void UBuildMenuElement::UpdateState(float NewMoney) const
{
    ElementButton->SetIsEnabled(NewMoney >= BuildingInfo.BuildCost);
}

void UBuildMenuElement::SetElementInfo(FBuildingInfo NewInfo)
{
    BuildingInfo = MoveTemp(NewInfo);
}

void UBuildMenuElement::NativeConstruct()
{
    Super::NativeConstruct();
    ElementButton->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
    OwnerController = GetOwningPlayer<ATowerDefensePlayerController>();
    check(OwnerController.IsValid());
}

void UBuildMenuElement::OnButtonClicked()
{
    if (const TStrongObjectPtr<ATowerDefensePlayerController> Owner = OwnerController.Pin(); Owner.IsValid())
    {
        Owner->SetBuildingMode(BuildingInfo);
    }
}
