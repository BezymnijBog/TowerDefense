// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru


#include "Widgets/MoneyWidget.h"

void UMoneyWidget::SetMoneyText(float NewMoney)
{
    MoneyCountText = FText::FromString(FString::FromInt(StaticCast<int32>(NewMoney)));
}