// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/BTTasks/BTTask_Attack.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UAbilitySystemComponent* const AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetOwner());
    if (!IsValid(AbilityComponent))
    {
        return EBTNodeResult::Failed;
    }

    AbilityComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(Action_Attack_Make));
    return EBTNodeResult::Succeeded;
}
