// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/PlayerHudWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Attributes/PlayerAttributeSet.h"
#include "Widgets/HireMenu.h"
#include "Widgets/MoneyWidget.h"
#include "Widgets/StartWaveWidget.h"
#include "Widgets/DayEventChoiceWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Utils/BaseUtils.h"

namespace
{
FText StatusText(const FRunState& State, ERunPhase Phase)
{
    switch (Phase)
    {
        case ERunPhase::RunComplete:
            return NSLOCTEXT("Run", "Complete", "Run complete!");
        case ERunPhase::RunFailed:
            return NSLOCTEXT("Run", "Failed", "Run failed!");
        case ERunPhase::PostEvent:
            return NSLOCTEXT("Run", "Settlement", "Reward received. Prepare your settlement, then continue.");
        default:
            return FText::Format(NSLOCTEXT("Run", "Day", "Act {0}, day {1}"), State.CurrentAct, State.CurrentDay);
    }
}
} // namespace

void UPlayerHudWidget::ToggleWaveWidgetVisibility(bool bIsVisible) const
{
    StartWave->SetVisibility(bIsVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UPlayerHudWidget::SetMoney(float NewMoney) const
{
    MoneyWidget->SetMoneyText(NewMoney);
    BuildMenu->UpdateHireSlots(NewMoney);
}

void UPlayerHudWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BuildMenu->InitializeFromTable();
    NextDayButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OnNextDayClicked);
    OnRunPhaseChanged(ERunPhase::None);
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->OnPhaseChanged.RemoveAll(this);
        Run->OnPhaseChanged.AddUObject(this, &ThisClass::OnRunPhaseChanged);
        OnRunPhaseChanged(Run->GetRunState().Phase);
    }
}

void UPlayerHudWidget::OnRunPhaseChanged(ERunPhase Phase)
{
    ToggleWaveWidgetVisibility(Phase == ERunPhase::CombatPreparation);
    NextDayButton->SetVisibility(Phase == ERunPhase::PostEvent ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    StatusText->SetVisibility(Phase == ERunPhase::None ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
    if (const URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        StatusText->SetText(::StatusText(Run->GetRunState(), Phase));
    }
}

void UPlayerHudWidget::NativeDestruct()
{
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->OnPhaseChanged.RemoveAll(this);
    }

    NextDayButton->OnClicked.RemoveDynamic(this, &ThisClass::OnNextDayClicked);
    Super::NativeDestruct();
}

void UPlayerHudWidget::OnAddedToViewport()
{
    if (APlayerController* Controller = GetOwningPlayer(); IsValid(Controller))
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        Controller->SetInputMode(InputMode);
        Controller->SetShowMouseCursor(true);
    }
    if (const UAbilitySystemComponent* AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayer()); IsValid(AbilitySystem))
    {
        SetMoney(AbilitySystem->GetSet<UPlayerAttributeSet>()->GetMoney());
    }
}

void UPlayerHudWidget::OnNextDayClicked()
{
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->AdvanceDay();
    }
}
