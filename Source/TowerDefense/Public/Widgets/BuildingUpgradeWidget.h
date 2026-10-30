// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "BuildingUpgradeWidget.generated.h"

class AActor;
class IConstructionTarget;
class UButton;

UCLASS(Abstract)
class TOWERDEFENSE_API UBuildingUpgradeWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    TAttribute<AActor*> ConstructionTarget;
    TAttribute<float> ConstructionCost;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    TObjectPtr<UButton> UpgradeButton;

private:
    void OnPlayerMoneyChanged(const FOnAttributeChangeData& OnAttributeChangeData);
    void UpdateButtonVisibility(float CurrentMoney);

    UFUNCTION()
    void Upgrade();

    FDelegateHandle Handle;
};
