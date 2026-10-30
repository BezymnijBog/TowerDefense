// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/BuildingUpgradeWidget.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemComponent.h"
#include "Attributes/PlayerAttributeSet.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Utils/BaseUtils.h"

namespace
{
FOnGameplayAttributeValueChange& GetMoneyChangeDelegate(UAbilitySystemComponent& AbilitySystemComponent)
{
    return AbilitySystemComponent.GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMoneyAttribute());
}
} // namespace

void UBuildingUpgradeWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UpgradeButton->OnClicked.AddUniqueDynamic(this, &ThisClass::Upgrade);
    if (UAbilitySystemComponent* AbilitySystem = UInterfaceFunctionLibrary::GetAbilitySystemComponent(GetOwningPlayer()); IsValid(AbilitySystem))
    {
        Handle = GetMoneyChangeDelegate(*AbilitySystem).AddUObject(this, &ThisClass::OnPlayerMoneyChanged);
        UpdateButtonVisibility(AbilitySystem->GetSet<UPlayerAttributeSet>()->GetMoney());
    }
}

void UBuildingUpgradeWidget::NativeDestruct()
{
    if (UAbilitySystemComponent* AbilitySystem = UInterfaceFunctionLibrary::GetAbilitySystemComponent(GetOwningPlayer()); IsValid(AbilitySystem))
    {
        GetMoneyChangeDelegate(*AbilitySystem).Remove(Handle);
    }
    UpgradeButton->OnClicked.RemoveDynamic(this, &ThisClass::Upgrade);
    Super::NativeDestruct();
}

void UBuildingUpgradeWidget::OnPlayerMoneyChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
    UpdateButtonVisibility(OnAttributeChangeData.NewValue);
}

void UBuildingUpgradeWidget::UpdateButtonVisibility(float CurrentMoney)
{
    UpgradeButton->SetVisibility(CurrentMoney >= ConstructionCost.Get() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UBuildingUpgradeWidget::Upgrade()
{
    AbilitySystem::SendGameplayEventToInstigator(GetOwningPlayer(), ConstructionTarget.Get(), Action_Building_Construct, ConstructionCost.Get());
}
