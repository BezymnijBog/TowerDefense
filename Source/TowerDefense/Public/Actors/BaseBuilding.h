// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "BaseBuilding.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UBoxComponent;
class UBuildingAttributeSet;
class UProgressBarWidget;
class UWidgetComponent;

UCLASS()
class TOWERDEFENSE_API ABaseBuilding : public AActor, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    ABaseBuilding();

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

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
    TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Tower|Abilities")
    TObjectPtr<const UBuildingAttributeSet> AttributeSet;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Tower|Abilities")
    TWeakObjectPtr<UProgressBarWidget> HealthWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AICharacter")
    FGenericTeamId TeamId;
};
