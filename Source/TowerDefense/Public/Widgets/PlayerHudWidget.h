// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHudWidget.generated.h"

class UBuildMenu;
class UMoneyWidget;

UCLASS()
class TOWERDEFENSE_API UPlayerHudWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetMoney(float NewMoney) const;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (BindWidget))
    TObjectPtr<UMoneyWidget> MoneyWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (BindWidget))
    TObjectPtr<UBuildMenu> BuildMenu;
};
