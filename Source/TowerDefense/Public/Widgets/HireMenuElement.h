// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/HireElementInterface.h"
#include "Utils/BaseUtils.h"
#include "HireMenuElement.generated.h"

class ATowerDefenseAICharacter;
class UButton;
class UImage;
class UTextBlock;

UCLASS()
class TOWERDEFENSE_API UHireMenuElement : public UUserWidget, public IHireElementInterface
{
    GENERATED_BODY()

public:
    virtual bool GetHireInfo(FHireInfo& OutInfo) override;
    void UpdateState(float NewMoney);
    void InitializeElement(const FHireWidgetRow* TableRow);

protected:
    UPROPERTY(EditAnywhere, Category = HireMenu)
    FHireInfo Info;

    UPROPERTY(EditAnywhere, Category = HireMenu, meta = (BindWidget))
    TObjectPtr<UButton> PressButton;

    UPROPERTY(EditAnywhere, Category = HireMenu, meta = (BindWidget))
    TObjectPtr<UTextBlock> CostText;

    UPROPERTY(EditAnywhere, Category = HireMenu, meta = (BindWidget))
    TObjectPtr<UImage> UnitImage;
};
