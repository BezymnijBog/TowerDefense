// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/UnitAbilitySystemComponent.h"

#include "Attributes/AICharacterAttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Equipment/UnitEquipmentAssembly.h"
#include "Equipment/UnitEquipmentEffect.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY_STATIC(LogUnitEquipment, Log, All);

void UUnitAbilitySystemComponent::ApplyEquipment()
{
    const ACharacter* Character = Cast<ACharacter>(GetOwner());
    USkeletalMesh* Mesh = Character && Character->GetMesh() ? Character->GetMesh()->GetSkeletalMeshAsset() : nullptr;
    const UUnitEquipmentAssembly* Assembly =
        Mesh ? Cast<UUnitEquipmentAssembly>(Mesh->GetAssetUserDataOfClass(UUnitEquipmentAssembly::StaticClass())) : nullptr;
    if (!Assembly || !IsValid(AttributeSet) || EquipmentEffectHandle.IsValid())
    {
        return;
    }
    FText Error;
    if (!Assembly->Loadout.Validate(Error))
    {
        UE_LOG(LogUnitEquipment, Error, TEXT("Cannot initialize equipment for %s: %s"), *GetNameSafe(GetOwner()), *Error.ToString());
        return;
    }
    const FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(UUnitEquipmentEffect::StaticClass(), 1.f, MakeEffectContext());
    if (!Spec.IsValid())
    {
        return;
    }
    TMap<FName, float> Bonuses;
    for (const UUnitEquipmentDefinition* Item : Assembly->Loadout.GetItems())
    {
        if (!Item)
        {
            continue;
        }
        Spec.Data->DynamicGrantedTags.AppendTags(Item->GrantedTags);
        for (const FUnitEquipmentModifier& Modifier : Item->Modifiers)
        {
            Bonuses.FindOrAdd(Modifier.Attribute.GetUProperty()->GetFName()) += Modifier.Bonus;
        }
    }
    if (Bonuses.IsEmpty() && Spec.Data->DynamicGrantedTags.IsEmpty())
    {
        return;
    }
    // Explicit zeroes prevent missing SetByCaller values for attributes with no equipment bonus.
    for (const FGameplayModifierInfo& Modifier : Spec.Data->Def->Modifiers)
    {
        const FName Name = Modifier.Attribute.GetUProperty()->GetFName();
        Spec.Data->SetSetByCallerMagnitude(Name, Bonuses.FindRef(Name));
    }
    const float PreviousMaxHitPoints = AttributeSet->GetStrength() + AttributeSet->GetExtraHitPoints();
    EquipmentEffectHandle = ApplyGameplayEffectSpecToSelf(*Spec.Data);
    if (EquipmentEffectHandle.IsValid())
    {
        const float MaxHitPointsDelta = AttributeSet->GetStrength() + AttributeSet->GetExtraHitPoints() - PreviousMaxHitPoints;
        const FGameplayAttribute HitPoints = UAICharacterAttributeSet::GetHitPointsAttribute();
        SetNumericAttributeBase(HitPoints, GetNumericAttributeBase(HitPoints) + MaxHitPointsDelta);
    }
}
