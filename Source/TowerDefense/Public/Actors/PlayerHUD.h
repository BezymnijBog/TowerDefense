// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class IPlayerHudInterface;
class UPlayerHudWidget;

UCLASS()
class TOWERDEFENSE_API APlayerHUD : public AHUD
{
    GENERATED_BODY()

public:
    template <std::derived_from<UUserWidget> T>
    T* GetHUDWidget() const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (MustImplement = "/Script/TowerDefense.PlayerHudInterface"))
    TSubclassOf<UUserWidget> HudWidgetClass;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "HUD")
    TScriptInterface<IPlayerHudInterface> HudWidget;
};

template <std::derived_from<UUserWidget> T>
T* APlayerHUD::GetHUDWidget() const
{
    return Cast<T>(HudWidget.GetObject());
}
