// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/PlayerHudInterface.h"
#include "Subsystems/RunSubsystem.h"
#include "PlayerHudWidget.generated.h"

class UDayEventChoiceWidget;
class UStartWaveWidget;
class UHireMenu;
class UMoneyWidget;
class UButton;
class UTextBlock;

UCLASS()
class TOWERDEFENSE_API UPlayerHudWidget : public UUserWidget, public IPlayerHudInterface
{
    GENERATED_BODY()

public:
    void ToggleWaveWidgetVisibility(bool bIsVisible) const;
    void SetMoney(float NewMoney) const;

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    virtual void OnAddedToViewport() override;

    void OnRunPhaseChanged(ERunPhase Phase);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> NextDayButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> StatusText;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (BindWidget))
    TObjectPtr<UDayEventChoiceWidget> DayChoiceWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (BindWidget))
    TObjectPtr<UMoneyWidget> MoneyWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (BindWidget))
    TObjectPtr<UHireMenu> BuildMenu;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (BindWidget))
    TObjectPtr<UStartWaveWidget> StartWave;

private:
    UFUNCTION()
    void OnNextDayClicked();
};
