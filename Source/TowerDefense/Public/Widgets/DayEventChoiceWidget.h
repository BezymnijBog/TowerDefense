// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/RunSubsystem.h"
#include "DayEventChoiceWidget.generated.h"

class UDayChoiceCard;

UCLASS()
class TOWERDEFENSE_API UDayEventChoiceWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDayChoiceCard> ChoiceCardLeft;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDayChoiceCard> ChoiceCardMiddle;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDayChoiceCard> ChoiceCardRight;

private:
    void OnDayChoiceGenerated(const TArray<FDayChoice>& DayChoices);
    void OnPhaseChanged(ERunPhase Phase);

    UPROPERTY()
    TArray<TObjectPtr<UDayChoiceCard>> Cards;

    FDelegateHandle DayChoiceHandle;
};
