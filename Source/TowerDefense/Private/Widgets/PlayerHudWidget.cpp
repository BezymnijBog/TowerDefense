// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/PlayerHudWidget.h"

#include "Widgets/HireMenu.h"
#include "Widgets/MoneyWidget.h"

void UPlayerHudWidget::SetMoney(float NewMoney) const
{
    MoneyWidget->SetMoneyText(NewMoney);
    BuildMenu->UpdateHireSlots(NewMoney);
}

void UPlayerHudWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BuildMenu->InitializeFromTable();
}
