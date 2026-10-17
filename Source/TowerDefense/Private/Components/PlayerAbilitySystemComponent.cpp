// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/PlayerAbilitySystemComponent.h"

#include "Actors/PlayerHUD.h"
#include "Attributes/PlayerAttributeSet.h"
#include "Utils/AbilityDescription.h"
#include "Widgets/PlayerHudWidget.h"

void UPlayerAbilitySystemComponent::BeginPlay()
{
    Super::BeginPlay();
    PlayerAttributes = AddSet<UPlayerAttributeSet>();
    GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMoneyAttribute()).AddUObject(this, &ThisClass::OnMoneyChanged);

    for (const FDefaultAbilityDescription& AbilityDescription : DefaultAbilities)
    {
        if (FGameplayAbilitySpecHandle AbilityHandle = GiveAbility(AbilityDescription.ToSpec()); AbilityDescription.bActivateAtStartup)
        {
            TryActivateAbility(MoveTemp(AbilityHandle));
        }
    }
}

void UPlayerAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CancelAllAbilities();
    Super::EndPlay(EndPlayReason);
}

void UPlayerAbilitySystemComponent::OnMoneyChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
    if (const APlayerHUD* const HUD = GetOwner<APlayerController>()->GetHUD<APlayerHUD>(); IsValid(HUD))
    {
        if (const UPlayerHudWidget* HudWidget = HUD->GetHUDWidget<UPlayerHudWidget>(); IsValid(HudWidget))
        {
            HudWidget->SetMoney(OnAttributeChangeData.NewValue);
        }
    }
}
