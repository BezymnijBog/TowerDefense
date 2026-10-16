// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/DayEventChoiceWidget.h"

#include "Utils/BaseUtils.h"
#include "Widgets/DayChoiceCard.h"

void UDayEventChoiceWidget::NativeConstruct()
{
    Super::NativeConstruct();
    Cards = { ChoiceCardLeft, ChoiceCardMiddle, ChoiceCardRight };
    SetVisibility(ESlateVisibility::Collapsed);

    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->DayChoiceGenerated.RemoveAll(this);
        DayChoiceHandle = Run->DayChoiceGenerated.AddUObject(this, &ThisClass::OnDayChoiceGenerated);
        Run->OnPhaseChanged.RemoveAll(this);
        Run->OnPhaseChanged.AddUObject(this, &ThisClass::OnPhaseChanged);
        OnPhaseChanged(Run->GetRunState().Phase);
    }
}

void UDayEventChoiceWidget::NativeDestruct()
{
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->DayChoiceGenerated.Remove(DayChoiceHandle);
        Run->OnPhaseChanged.RemoveAll(this);
    }
    Super::NativeDestruct();
}

void UDayEventChoiceWidget::OnDayChoiceGenerated(const TArray<FDayChoice>& DayChoices)
{
    if (DayChoices.IsEmpty())
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }
    if (DayChoices.Num() == 1)
    {
        ChoiceCardLeft->SetVisibility(ESlateVisibility::Collapsed);
        ChoiceCardMiddle->Generate(DayChoices.Last());
        ChoiceCardRight->SetVisibility(ESlateVisibility::Collapsed);
    }
    else if (DayChoices.Num() == 2)
    {
        ChoiceCardLeft->Generate(DayChoices[0]);
        ChoiceCardMiddle->SetVisibility(ESlateVisibility::Collapsed);
        ChoiceCardRight->Generate(DayChoices[1]);
    }
    else
    {
        ensure(DayChoices.Num() == 3);
        for (int32 Index = 0; Index < Cards.Num(); ++Index)
        {
            Cards[Index]->Generate(DayChoices[Index]);
        }
    }
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDayEventChoiceWidget::OnPhaseChanged(ERunPhase Phase)
{
    SetVisibility(ESlateVisibility::Collapsed);
    if (const URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this);
        Phase == ERunPhase::DaySelection && IsValid(Run))
    {
        OnDayChoiceGenerated(Run->GetCurrentChoices());
    }
}
