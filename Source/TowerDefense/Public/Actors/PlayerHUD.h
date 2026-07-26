// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UPlayerHudWidget;

UCLASS()
class TOWERDEFENSE_API APlayerHUD : public AHUD
{
    GENERATED_BODY()

public:
    UPlayerHudWidget* GetHUDWidget() const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HUD")
    TSubclassOf<UPlayerHudWidget> HudWidgetClass;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="HUD")
    TObjectPtr<UPlayerHudWidget> HudWidget;
};
