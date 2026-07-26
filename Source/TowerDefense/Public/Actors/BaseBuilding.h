// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "BaseBuilding.generated.h"

class UBoxComponent;
class UProgressBarWidget;
class UBuildingAttributeSet;
class UWidgetComponent;

UCLASS()
class TOWERDEFENSE_API ABaseBuilding : public AActor, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ABaseBuilding();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
    void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> HitBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> WidgetComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Tower|Abilities")
    TObjectPtr<const UBuildingAttributeSet> AttributeSet;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Tower|Abilities")
    TWeakObjectPtr<UProgressBarWidget> HealthWidget;
};
