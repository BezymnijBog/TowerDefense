// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/BTTasks/BTTask_Attack.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AI/TowerDefenseAIController.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UAbilitySystemComponent* const AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetOwner());
    const ATowerDefenseAIController* Controller = Cast<ATowerDefenseAIController>(OwnerComp.GetAIOwner());
    if (!IsValid(AbilityComponent) || !IsValid(Controller) || !Controller->CanAttackCurrentTarget())
    {
        return EBTNodeResult::Failed;
    }

    AbilityComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(Action_Attack_Make));
    return EBTNodeResult::Succeeded;
}
