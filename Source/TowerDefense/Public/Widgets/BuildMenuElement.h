// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Utils/BaseUtils.h"
#include "BuildMenuElement.generated.h"

class ATowerDefensePlayerController;
class ATurret;
class UButton;

UCLASS()
class TOWERDEFENSE_API UBuildMenuElement : public UUserWidget
{
    GENERATED_BODY()

public:
    float GetCost() const;

    void UpdateState(float NewMoney) const;

    void SetElementInfo(FBuildingInfo NewInfo);

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
    TObjectPtr<UButton> ElementButton;

    UPROPERTY(EditDefaultsOnly, Category = "BuildMenuElement")
    FBuildingInfo BuildingInfo;

private:
    UFUNCTION()
    void OnButtonClicked();

    UPROPERTY()
    TWeakObjectPtr<ATowerDefensePlayerController> OwnerController;
};
