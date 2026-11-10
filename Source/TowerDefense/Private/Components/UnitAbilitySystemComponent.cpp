// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/UnitAbilitySystemComponent.h"

#include "Attributes/AICharacterAttributeSet.h"
#include "Utils/AbilityDescription.h"
#include "Utils/BaseUtils.h"

void UUnitAbilitySystemComponent::BeginPlay()
{
    Super::BeginPlay();

    AddLooseGameplayTags(DefaultTags);

    AttributeSet = GetSet<UAICharacterAttributeSet>();
    GetGameplayAttributeValueChangeDelegate(UAICharacterAttributeSet::GetHitPointsAttribute()).AddUObject(this, &ThisClass::OnHitPointsChanged);
    GetGameplayAttributeValueChangeDelegate(UAICharacterAttributeSet::GetStrengthAttribute()).AddUObject(this, &ThisClass::OnStrengthChanged);

    {
        TGuardValue<bool> InitializationGuard(bInitializingEquipment, true);
        ApplyEquipment();
    }

    for (const FDefaultAbilityDescription& AbilityDescription : DefaultAbilities)
    {
        if (FGameplayAbilitySpecHandle AbilitySpec = GiveAbility(AbilityDescription.ToSpec()); AbilityDescription.bActivateAtStartup)
        {
            TryActivateAbility(MoveTemp(AbilitySpec));
        }
    }
}

void UUnitAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetGameplayAttributeValueChangeDelegate(UAICharacterAttributeSet::GetHitPointsAttribute()).RemoveAll(this);
    GetGameplayAttributeValueChangeDelegate(UAICharacterAttributeSet::GetStrengthAttribute()).RemoveAll(this);
    if (EquipmentEffectHandle.IsValid())
    {
        RemoveActiveGameplayEffect(EquipmentEffectHandle);
        EquipmentEffectHandle.Invalidate();
    }
    Super::EndPlay(EndPlayReason);
}

void UUnitAbilitySystemComponent::OnHitPointsChanged(const FOnAttributeChangeData& Data)
{
    if (bIsDead || bInitializingEquipment || !IsValid(AttributeSet))
    {
        return;
    }

    const float MaxHitPoints = AttributeSet->GetExtraHitPoints() + AttributeSet->GetStrength();
    const float OldHitPoints = Data.OldValue;
    const float NewHitPoints = Data.NewValue;

    const int32 GuaranteedDeathThreshold = FMath::FloorToInt(FMath::Clamp(AttributeSet->GetDeathThresholds(), 1.f, 5.f));
    const float GuaranteedDeathHitPoints = -GuaranteedDeathThreshold * MaxHitPoints;
    if (bIsDead = NewHitPoints <= GuaranteedDeathHitPoints; bIsDead)
    {
        std::ignore = UnitDead.ExecuteIfBound();
        return;
    }

    // Check equality in case of strength change, that can also cause crossing the death threshold
    if (NewHitPoints <= OldHitPoints)
    {
        for (int32 NextDeathThreshold = CurrentDeathThreshold + 1; NextDeathThreshold < GuaranteedDeathThreshold; ++NextDeathThreshold)
        {
            if (const double ThresholdHitPoints = -NextDeathThreshold * MaxHitPoints; NewHitPoints < ThresholdHitPoints)
            {
                CurrentDeathThreshold = NextDeathThreshold;
                std::ignore = DeathThresholdExceeded.ExecuteIfBound(CurrentDeathThreshold);
                bIsDead |= !RollUtils::IsCheckSuccessful(FMath::FloorToInt(AttributeSet->GetHealth()));
            }
            else
            {
                break;
            }
        }
    }
    // Check equality and don't add 'else' here for the same reason as above
    if (NewHitPoints >= OldHitPoints)
    {
        for (int32 PrevDeathThreshold = CurrentDeathThreshold - 1; PrevDeathThreshold >= 0; --PrevDeathThreshold)
        {
            if (const double ThresholdHitPoints = -PrevDeathThreshold * MaxHitPoints; NewHitPoints > ThresholdHitPoints)
            {
                CurrentDeathThreshold = PrevDeathThreshold;
                std::ignore = DeathThresholdExceeded.ExecuteIfBound(CurrentDeathThreshold);
            }
            else
            {
                break;
            }
        }
    }

    std::ignore =
        HitPointPercentChanged.ExecuteIfBound(FMath::Clamp((NewHitPoints - GuaranteedDeathHitPoints) / (MaxHitPoints - GuaranteedDeathHitPoints), 0.f, 1.f));
    if (bIsDead)
    {
        std::ignore = UnitDead.ExecuteIfBound();
    }
}

void UUnitAbilitySystemComponent::OnStrengthChanged(const FOnAttributeChangeData& Data)
{
    if (!IsValid(AttributeSet))
    {
        return;
    }
    FOnAttributeChangeData HitPointsChangeData;
    HitPointsChangeData.Attribute = UAICharacterAttributeSet::GetHitPointsAttribute();
    HitPointsChangeData.NewValue = AttributeSet->GetHitPoints();
    HitPointsChangeData.OldValue = HitPointsChangeData.NewValue;
    OnHitPointsChanged(HitPointsChangeData);

    const FGameplayAttribute DamageAttribute = UAICharacterAttributeSet::GetDamageAttribute();
    SetNumericAttributeBase(DamageAttribute, Data.NewValue - Data.OldValue + GetNumericAttributeBase(DamageAttribute));
}
