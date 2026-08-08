// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/HireMenu.h"

#include "Components/UniformGridPanel.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Utils/BaseUtils.h"
#include "Widgets/HireMenuElement.h"

void UHireMenu::InitializeFromTable()
{
    if (!IsValid(InitializeTable))
    {
        return;
    }

    TArray<FHireWidgetRow*> Rows;
    InitializeTable->GetAllRows(TEXT("WidgetInitialize"), Rows);
    for (int32 Idx = 0; Idx < Rows.Num(); ++Idx)
    {
        UHireMenuElement* const Element = CreateWidget<UHireMenuElement>(this, ElementsClass);
        Element->InitializeElement(Rows[Idx]);
        GridPanel->AddChildToUniformGrid(Element, Idx % GridRowsNum, Idx / GridRowsNum);
    }
}

void UHireMenu::UpdateHireSlots(float NewMoney) const
{
    for (const UPanelSlot* const PanelSlot : GridPanel->GetSlots())
    {
        if (!IsValid(PanelSlot) || !IsValid(PanelSlot->GetContent()))
        {
            continue;
        }

        UWidget* const SlotWidget = PanelSlot->GetContent();
        FHireInfo Info;
        SlotWidget->SetIsEnabled(UInterfaceFunctionLibrary::GetHireInfo(SlotWidget, Info) && NewMoney >= Info.Cost);
    }
}
