// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utils/BaseUtils.h"
#include "BuildMenu.generated.h"

class UUniformGridPanel;
class UBuildMenuElement;

UCLASS()
class TOWERDEFENSE_API UBuildMenu : public UUserWidget
{
    GENERATED_BODY()

public:
    void UpdateBuildingAbility(float NewMoney) const;
    virtual bool Initialize() override;

protected:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;

    UPROPERTY(EditDefaultsOnly, Category = "Build Menu")
    FBuildingInfo AttackingTurretInfo;

    UPROPERTY(EditDefaultsOnly, Category = "Build Menu")
    FBuildingInfo BuffingTurretInfo;

    UPROPERTY(EditDefaultsOnly, Category = "Build Menu")
    FBuildingInfo DebuffingTurretInfo;

    UPROPERTY(BlueprintReadOnly, Category = "Build Menu", meta = (BindWidget))
    TObjectPtr<UBuildMenuElement> AttackingTurret;

    UPROPERTY(BlueprintReadOnly, Category = "Build Menu", meta = (BindWidget))
    TObjectPtr<UBuildMenuElement> BuffingTurret;

    UPROPERTY(BlueprintReadOnly, Category = "Build Menu", meta = (BindWidget))
    TObjectPtr<UBuildMenuElement> DebuffingTurret;

    UPROPERTY(BlueprintReadOnly, Category = "Build Menu", meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> BuildGrid;

private:
    TArray<UBuildMenuElement*> GetElements() const;
};
