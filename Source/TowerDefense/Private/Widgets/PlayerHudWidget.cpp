// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru


#include "Widgets/PlayerHudWidget.h"

#include "Widgets/BuildMenu.h"
#include "Widgets/MoneyWidget.h"

void UPlayerHudWidget::SetMoney(float NewMoney) const
{
    MoneyWidget->SetMoneyText(NewMoney);
    BuildMenu->UpdateBuildingAbility(NewMoney);
}
