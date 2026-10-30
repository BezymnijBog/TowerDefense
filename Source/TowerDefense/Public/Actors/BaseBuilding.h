// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/CellPlacedInterface.h"
#include "Interfaces/DeathInterface.h"
#include "Interfaces/ConstructionTarget.h"
#include "BaseBuilding.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UBuildingConstructionComponent;
class UBoxComponent;
class UBuildingAttributeSet;
class UProgressBarWidget;
class UWidgetComponent;

UCLASS()
class TOWERDEFENSE_API ABaseBuilding :
    public AActor,
    public IAbilitySystemInterface,
    public IGenericTeamAgentInterface,
    public IDeathInterface,
    public ICellPlacedInterface,
    public IConstructionTarget
{
    GENERATED_BODY()

public:
    ABaseBuilding();

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual bool IsDead() const override;
    virtual void OnDeath() override;
    virtual FDeathDelegate& GetDeathDelegate() override;

    virtual const TArray<FIntVector2>& GetAdjacentCells() const override;
    virtual const TArray<FIntVector2>& GetOccupiedCells() const override;
    virtual FIntVector2 GetSize() const override;

    UBuildingConstructionComponent* GetConstructionComponent() const { return Construction; }
    virtual void StartConstruction() override;
    virtual bool CanStartConstruction() const override;

protected:
    void ApplyConstructionState(bool bActive, UStaticMesh* StageMesh);

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UBuildingConstructionComponent> Construction;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UWidgetComponent> UpgradeWidget;

    void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);

    // Called once after the building becomes ruins, before notifying death listeners.
    virtual void OnBuildingDestroyed() {}

    virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> HitBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> WidgetComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "BaseBuilding|Abilities")
    TObjectPtr<const UBuildingAttributeSet> AttributeSet;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "BaseBuilding|Abilities")
    TWeakObjectPtr<UProgressBarWidget> HealthWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseBuilding")
    FGenericTeamId TeamId;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "BaseBuilding")
    bool bIsDead = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseBuilding")
    float AttackSlotRadius = 34.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseBuilding")
    float AttackSlotHeight = 88.0f;

    UPROPERTY(VisibleInstanceOnly, Category = "BaseBuilding")
    TArray<FIntVector2> OccupiedCells;

    UPROPERTY(VisibleInstanceOnly, Category = "BaseBuilding")
    TArray<FIntVector2> AdjacentCells;

    UPROPERTY(EditDefaultsOnly, Category = "BaseBuilding")
    FIntVector2 CellsSize = { 1, 1 };

private:
    FDeathDelegate DeathDelegate;
};
