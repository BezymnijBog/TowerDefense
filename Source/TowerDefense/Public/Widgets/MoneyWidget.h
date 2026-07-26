// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MoneyWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class TOWERDEFENSE_API UMoneyWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetMoneyText(float NewMoney);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MoneyWidget", meta = (BindWidget))
    TObjectPtr<UImage> MoneyImage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MoneyWidget", meta = (BindWidget))
    TObjectPtr<UTextBlock> MoneyCount;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "MoneyWidget")
    FText MoneyCountText;
};
