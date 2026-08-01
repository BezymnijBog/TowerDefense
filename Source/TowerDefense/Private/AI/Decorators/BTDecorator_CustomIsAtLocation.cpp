// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/Decorators/BTDecorator_CustomIsAtLocation.h"

void UBTDecorator_CustomIsAtLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    AcceptableRadius = AcceptanceRadius.GetValue(OwnerComp);
}
