// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/PlayerHUD.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Attributes/PlayerAttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/PlayerHudWidget.h"

UPlayerHudWidget* APlayerHUD::GetHUDWidget() const
{
    return HudWidget;
}

void APlayerHUD::BeginPlay()
{
    Super::BeginPlay();

    HudWidget = CreateWidget<UPlayerHudWidget>(GetWorld(), HudWidgetClass, TEXT("PlayerHUD"));
    HudWidget->AddToViewport();

    if (const UAbilitySystemComponent* const AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerController()))
    {
        HudWidget->SetMoney(AbilitySystem->GetSet<UPlayerAttributeSet>()->GetMoney());
    }
}

void APlayerHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    HudWidget->RemoveFromParent();
    Super::EndPlay(EndPlayReason);
}