// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/PlayerHUD.h"

#include "Blueprint/UserWidget.h"
#include "Interfaces/PlayerHudInterface.h"
#include "GameFramework/PlayerController.h"

void APlayerHUD::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* Controller = GetOwningPlayerController();
    if (!IsValid(Controller) || !Controller->IsLocalController()
        || !ensure(HudWidgetClass && HudWidgetClass->ImplementsInterface(UPlayerHudInterface::StaticClass())))
    {
        return;
    }

    if (UUserWidget* Widget = CreateWidget<UUserWidget>(Controller, HudWidgetClass, TEXT("PlayerHUD")); IsValid(Widget))
    {
        HudWidget = Widget;
        if (ensure(HudWidget.GetInterface()))
        {
            Widget->AddToViewport();
            HudWidget->OnAddedToViewport();
        }
    }
}

void APlayerHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UUserWidget* Widget = GetHUDWidget<UUserWidget>(); IsValid(Widget))
    {
        Widget->RemoveFromParent();
    }
    HudWidget = nullptr;
    Super::EndPlay(EndPlayReason);
}
