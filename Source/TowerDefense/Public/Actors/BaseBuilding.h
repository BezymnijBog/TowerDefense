// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/AttackSlotTarget.h"
#include "Interfaces/CellPlacedInterface.h"
#include "Interfaces/DeathInterface.h"
#include "BaseBuilding.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UAttackSlotComponent;
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
    public IAttackSlotTarget,
    public ICellPlacedInterface
{
    GENERATED_BODY()

public:
    ABaseBuilding();

    virtual void OnConstruction(const FTransform& Transform) override;

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual TArray<FAttackSlot> GetSlotPoints() const override;

    virtual bool IsDead() const override;
    virtual void OnDeath() override;
    virtual FDeathDelegate& GetDeathDelegate() override;

    virtual TArray<FIntVector2> GetAdjacentCells() const override;
    virtual TArray<FIntVector2> GetOccupiedCells() const override;
    virtual FIntVector2 GetSize() const override;

protected:
    void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);

    virtual void BeginPlay() override;

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

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UAttackSlotComponent> SlotsComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "BaseBuilding|Abilities")
    TObjectPtr<const UBuildingAttributeSet> AttributeSet;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "BaseBuilding|Abilities")
    TWeakObjectPtr<UProgressBarWidget> HealthWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseBuilding")
    FGenericTeamId TeamId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseBuilding")
    bool bIsDead = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseBuilding")
    float TimeBeforeDestroy = 2.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseBuilding")
    float AttackSlotRadius = 34.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseBuilding")
    float AttackSlotHeight = 88.0f;

    UPROPERTY(EditDefaultsOnly, Category = "BaseBuilding")
    FIntVector2 CellsSize = { 1, 1 };

private:
    virtual void InitializeSlots() override;
    void OnDeathTimerElapsed();

    FDeathDelegate DeathDelegate;
    FTimerHandle DeathTimerHandle;
};
