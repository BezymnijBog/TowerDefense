// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru


#include "Widgets/ProgressBarWidget.h"

void UProgressBarWidget::SetPercent(float Percent)
{
    ProgressPercent = FMath::Clamp(Percent, 0.f, 1.f);
}
