// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeSet.h"
#include "Interfaces/ConstructionTarget.h"
#include "BuildingConstructionComponent.generated.h"

class UStaticMesh;

USTRUCT()
struct FBuildingTier
{
    GENERATED_BODY()

    // Tier one can use the building's existing Mesh component instead.
    UPROPERTY(EditDefaultsOnly, Category = "Construction")
    TObjectPtr<UStaticMesh> FinishedMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Construction")
    TObjectPtr<UStaticMesh> ConstructionMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Construction")
    TObjectPtr<UStaticMesh> DestroyedMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Construction", meta = (ClampMin = "1"))
    int32 Days = 1;
};

DECLARE_DELEGATE_TwoParams(FApplyBuildingState, bool, UStaticMesh*);

UCLASS()
class TOWERDEFENSE_API UBuildingConstructionComponent : public UActorComponent, public IConstructionTarget
{
    GENERATED_BODY()

public:
    UBuildingConstructionComponent();

    virtual bool CanStartConstruction() const override;
    virtual void StartConstruction() override;

    UStaticMesh* GetStartingMesh() const;
    UStaticMesh* GetDestroyedMesh() const;

    void CancelConstruction();
    bool IsConstructing() const { return RemainingDays > 0; }
    int32 GetNextTierIndex() const { return NextTierIdx; }
    int32 GetRemainingDays() const { return RemainingDays; }
    const FBuildingTier* GetNextTier() const;

    FApplyBuildingState ApplyBuildingState;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditDefaultsOnly, EditFixedSize, Category = "Construction")
    TArray<FBuildingTier> Tiers;

    UPROPERTY(EditDefaultsOnly, Category = "Construction")
    int32 StartingTier = 0;

    UPROPERTY(EditDefaultsOnly, Category = "Construction")
    bool bStartsConstructed = false;

    UPROPERTY(VisibleInstanceOnly, Category = "Construction")
    int32 NextTierIdx = 0;

    UPROPERTY(VisibleInstanceOnly, Category = "Construction")
    int32 RemainingDays = 0;

private:
    bool CanProceedToNextTier() const;
    void OnDayEnded();

    bool bChangingState = false;
    FDelegateHandle OnDayEndedHandle;
};
