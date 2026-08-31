// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_CellAvailable.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API UEnvQueryTest_CellAvailable : public UEnvQueryTest
{
    GENERATED_BODY()

public:
    UEnvQueryTest_CellAvailable(const FObjectInitializer& ObjectInitializer);

    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

    virtual FText GetDescriptionDetails() const override;

protected:
    UPROPERTY(EditAnywhere, Category = SlotAvailable)
    bool bRejectOccupiedSlots = false;
};
