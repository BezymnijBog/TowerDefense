// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_IsAtLocation.h"
#include "BTDecorator_CustomIsAtLocation.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API UBTDecorator_CustomIsAtLocation : public UBTDecorator_IsAtLocation
{
    GENERATED_BODY()

public:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = Condition, meta = (EditCondition = "!bUseParametrizedRadius"))
    FValueOrBBKey_Float AcceptanceRadius;
};
