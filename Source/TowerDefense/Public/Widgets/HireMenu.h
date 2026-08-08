// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HireMenu.generated.h"

class UHireMenuElement;
class UUniformGridPanel;

UCLASS()
class TOWERDEFENSE_API UHireMenu : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeFromTable();
    void UpdateHireSlots(float NewMoney) const;

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Hire Menu", meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> GridPanel;

    UPROPERTY(EditDefaultsOnly, Category = "Hire Menu")
    TObjectPtr<UDataTable> InitializeTable;

    UPROPERTY(EditDefaultsOnly, Category = "Hire Menu")
    TSubclassOf<UHireMenuElement> ElementsClass;

    UPROPERTY(EditDefaultsOnly, Category = "Hire Menu")
    int32 GridRowsNum = 4;
};
