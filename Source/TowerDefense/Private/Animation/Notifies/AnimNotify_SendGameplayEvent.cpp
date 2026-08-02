// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Animation/Notifies/AnimNotify_SendGameplayEvent.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Utils/BaseUtils.h"

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    AActor* const Instigator = MeshComp->GetOwner();
    AAIController* const Controller = Instigator->GetInstigatorController<AAIController>();
    if (!IsValid(Controller))
    {
        return;
    }

    AActor* const Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKey));
    if (!IsValid(Target))
    {
        return;
    }

    AbilitySystem::SendGameplayEventToInstigator(Instigator, Target, Tag);
}
