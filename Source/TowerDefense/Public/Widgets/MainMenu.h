// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/PlayerHudInterface.h"
#include "MainMenu.generated.h"

class UButton;

UCLASS()
class TOWERDEFENSE_API UMainMenu : public UUserWidget, public IPlayerHudInterface
{
    GENERATED_BODY()

public:
    virtual void OnAddedToViewport() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
    TObjectPtr<UButton> NewGameButton;

    UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
    TObjectPtr<UButton> ExitButton;

private:
    void OnGameStartFailed();

    UFUNCTION()
    void OnNewGameClicked();

    UFUNCTION()
    void OnExitClicked();

    bool bStartingGame = false;
};
