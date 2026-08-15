// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/Tasks/AbilityTask_TraceCursor.h"

UAbilityTask_TraceCursor* UAbilityTask_TraceCursor::TraceCursorTask(UGameplayAbility* OwningAbility, FName InstanceName, APlayerController* TrackedController)
{
    UAbilityTask_TraceCursor* const NewTask = NewAbilityTask<ThisClass>(OwningAbility, InstanceName);
    NewTask->TrackingController = TrackedController;
    return NewTask;
}

UAbilityTask_TraceCursor::UAbilityTask_TraceCursor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    bTickingTask = true;
}

void UAbilityTask_TraceCursor::Activate()
{
    Super::Activate();
}

void UAbilityTask_TraceCursor::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);

    if (FHitResult HitResult; TrackingController.IsValid() && TrackingController->GetHitResultUnderCursor(ECC_GameTraceChannel1, false, HitResult))
    {
        std::ignore = CursorPositionChanged.ExecuteIfBound(HitResult);
    }
}
