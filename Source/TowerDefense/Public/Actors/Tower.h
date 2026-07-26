// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "Tower.generated.h"

class UBoxComponent;
class UProgressBarWidget;
class UTowerAttributeSet;
class UWidgetComponent;

UCLASS()
class TOWERDEFENSE_API ATower : public AActor, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ATower();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
    void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> TowerHitBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> TowerMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> WidgetComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Tower|Abilities")
    TObjectPtr<const UTowerAttributeSet> AttributeSet;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Tower|Abilities")
    TWeakObjectPtr<UProgressBarWidget> HealthWidget;

private:
    UFUNCTION()
    void OnEnemyEntered(UPrimitiveComponent* OverlappedComponent,
                        AActor* OtherActor,
                        UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex,
                        bool bFromSweep,
                        const FHitResult& SweepResult);
};
