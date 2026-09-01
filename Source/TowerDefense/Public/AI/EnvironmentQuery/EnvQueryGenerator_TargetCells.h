// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "DataProviders/AIDataProvider.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "EnvQueryGenerator_TargetCells.generated.h"

class UAISense;

/**
 * Gathers slots around actors perceived by context
 * Similar to UEnvQueryGenerator_PerceivedActors, but gathers result IAttackSlotTarget::GetSlotPoints
 */
UCLASS()
class TOWERDEFENSE_API UEnvQueryGenerator_TargetCells : public UEnvQueryGenerator
{
    GENERATED_BODY()

public:
    explicit UEnvQueryGenerator_TargetCells(const FObjectInitializer& ObjectInitializer);

protected:
    UPROPERTY(EditDefaultsOnly, Category = Generator)
    TSubclassOf<AActor> AllowedActorClass;

    UPROPERTY(EditDefaultsOnly, Category = Generator)
    FAIDataProviderFloatValue SearchRadius;

    UPROPERTY(EditAnywhere, Category = Generator)
    TSubclassOf<UEnvQueryContext> ListenerContext;

    UPROPERTY(EditAnywhere, Category = Generator)
    TSubclassOf<UAISense> SenseToUse;

    virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;

private:
    TArray<AActor*> GetListeners(FEnvQueryInstance& QueryInstance) const;
    TSet<AActor*> GetPerceivedActors(FEnvQueryInstance& QueryInstance) const;
    bool IsValidPerceivedActor(const AActor* Actor) const;
};
