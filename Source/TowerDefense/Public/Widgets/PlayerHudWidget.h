// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHudWidget.generated.h"

class UStartWaveWidget;
class UHireMenu;
class UMoneyWidget;

UCLASS()
class TOWERDEFENSE_API UPlayerHudWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void ToggleWaveWidgetVisibility(bool bIsVisible) const;
    void SetMoney(float NewMoney) const;
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (BindWidget))
    TObjectPtr<UMoneyWidget> MoneyWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (BindWidget))
    TObjectPtr<UHireMenu> BuildMenu;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (BindWidget))
    TObjectPtr<UStartWaveWidget> StartWave;
};
