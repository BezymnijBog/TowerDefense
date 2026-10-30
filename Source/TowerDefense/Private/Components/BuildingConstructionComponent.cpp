// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/BuildingConstructionComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Subsystems/RunSubsystem.h"
#include "Utils/BaseUtils.h"

UBuildingConstructionComponent::UBuildingConstructionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UBuildingConstructionComponent::CanStartConstruction() const
{
    const AActor* Owner = GetOwner();
    const URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(Owner);
    return IsValid(Run) && !Run->IsAdvancingDay() && UInterfaceFunctionLibrary::IsValidAndAlive(Owner) && CanProceedToNextTier();
}

void UBuildingConstructionComponent::StartConstruction()
{
    TGuardValue<bool> Guard(bChangingState, true);
    const FBuildingTier& NextTier = *GetNextTier();
    RemainingDays = FMath::Max(1, NextTier.Days);
    ApplyBuildingState.ExecuteIfBound(true, NextTier.ConstructionMesh);
}

UStaticMesh* UBuildingConstructionComponent::GetStartingMesh() const
{
    if (!Tiers.IsValidIndex(StartingTier))
    {
        return nullptr;
    }

    const FBuildingTier& StartTier = Tiers[StartingTier];
    return bStartsConstructed ? StartTier.FinishedMesh : StartTier.DestroyedMesh;
}

UStaticMesh* UBuildingConstructionComponent::GetDestroyedMesh() const
{
    const int32 TierIndex = FMath::Max(NextTierIdx - 1, 0);
    return Tiers.IsValidIndex(TierIndex) ? Tiers[TierIndex].DestroyedMesh.Get() : nullptr;
}

const FBuildingTier* UBuildingConstructionComponent::GetNextTier() const
{
    return Tiers.IsValidIndex(NextTierIdx) ? &Tiers[NextTierIdx] : nullptr;
}

void UBuildingConstructionComponent::CancelConstruction()
{
    RemainingDays = 0;
    NextTierIdx = 0;
}

void UBuildingConstructionComponent::BeginPlay()
{
    Super::BeginPlay();
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(GetOwner()); IsValid(Run))
    {
        OnDayEndedHandle = Run->OnDayEnded.AddUObject(this, &ThisClass::OnDayEnded);
    }
    if (bStartsConstructed)
    {
        ++NextTierIdx;
    }
}

void UBuildingConstructionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(GetOwner()); IsValid(Run))
    {
        Run->OnDayEnded.Remove(OnDayEndedHandle);
    }
    Super::EndPlay(EndPlayReason);
}

bool UBuildingConstructionComponent::CanProceedToNextTier() const
{
    return GetNextTier() != nullptr && !IsConstructing() && !bChangingState;
}

void UBuildingConstructionComponent::OnDayEnded()
{
    if (!IsConstructing() || !IsValid(GetOwner()) || UInterfaceFunctionLibrary::IsActorDead(GetOwner()))
    {
        return;
    }
    if (--RemainingDays == 0)
    {
        const FBuildingTier& Tier = Tiers[NextTierIdx++];
        ApplyBuildingState.ExecuteIfBound(true, Tier.FinishedMesh);
    }
}
