// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/DayChoiceCard.h"

#include "Subsystems/RunSubsystem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Utils/BaseUtils.h"

namespace
{
const TMap<EDayEventType, FString> EventTypeTitles{ { EDayEventType::NormalRaid, TEXT("Raid") },
                                                    { EDayEventType::EliteRaid, TEXT("Elite Raid") },
                                                    { EDayEventType::MysteryEvent, TEXT("Mystery Event") },
                                                    { EDayEventType::Merchant, TEXT("Merchant") },
                                                    { EDayEventType::BossRaid, TEXT("Boss Raid") } };
}

void UDayChoiceCard::Generate(const FDayChoice& DayChoice)
{
    Choice = DayChoice;
    if (DayChoice.EventType == EDayEventType::None)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    SetVisibility(ESlateVisibility::Visible);
    EventIcon->SetBrushFromTexture(DayChoice.Icon);
    TitleText->SetText(FText::FromString(EventTypeTitles.FindChecked(DayChoice.EventType)));
    DescriptionText->SetText(DayChoice.Description);
    RewardText->SetText(FText::Format(NSLOCTEXT("Run", "MoneyReward", "{0}"), FText::AsNumber(DayChoice.MoneyReward)));
    DifficultyText->SetText(FText::Format(NSLOCTEXT("Run", "Difficulty", "Difficulty: x{0}"), FText::AsNumber(DayChoice.Difficulty)));
}

void UDayChoiceCard::NativeConstruct()
{
    Super::NativeConstruct();
    if (SelectButton)
    {
        SelectButton->OnClicked.AddUniqueDynamic(this, &ThisClass::SelectChoice);
    }
}

void UDayChoiceCard::NativeDestruct()
{
    if (SelectButton)
    {
        SelectButton->OnClicked.RemoveDynamic(this, &ThisClass::SelectChoice);
    }
    Super::NativeDestruct();
}

void UDayChoiceCard::SelectChoice()
{
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->SelectDayEvent(Choice.EventId);
    }
}
