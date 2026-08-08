// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/HireMenuElement.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

bool UHireMenuElement::GetHireInfo(FHireInfo& OutInfo)
{
    OutInfo = Info;
    return true;
}

void UHireMenuElement::UpdateState(float NewMoney)
{
    SetIsEnabled(NewMoney >= Info.Cost);
}

void UHireMenuElement::InitializeElement(const FHireWidgetRow* TableRow)
{
    if (!TableRow)
    {
        return;
    }

    Info.Class = TableRow->Class;
    Info.Cost = TableRow->Cost;
    UnitImage->SetBrushResourceObject(TableRow->Image);
    CostText->SetText(FText::AsNumber(Info.Cost));
    PressButton->SetToolTipText(TableRow->Name);
}
