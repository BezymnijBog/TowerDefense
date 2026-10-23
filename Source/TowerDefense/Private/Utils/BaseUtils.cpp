// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Utils/BaseUtils.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interfaces/InterfaceFunctionLibrary.h"

namespace
{
FGameplayEventData MakePayload(AActor* Instigator, AActor* Target, float Magnitude)
{
    FGameplayEventData Payload;
    Payload.Instigator = Instigator;
    Payload.Target = Target;
    Payload.EventMagnitude = Magnitude;
    return Payload;
}
} // namespace

namespace Teams
{
ETeamAttitude::Type AttitudeSolveFunction(FGenericTeamId Left, FGenericTeamId Right)
{
    if (Left == Right)
    {
        return ETeamAttitude::Friendly;
    }

    if (Left == TeamNone || Right == TeamNone)
    {
        return ETeamAttitude::Neutral;
    }

    return ETeamAttitude::Hostile;
}

bool CanAttackEachOther(const AActor* Left, const AActor* Right)
{
    return UInterfaceFunctionLibrary::IsValidAndAlive(Left) && UInterfaceFunctionLibrary::IsValidAndAlive(Right)
        && FGenericTeamId::GetAttitude(Left, Right) == ETeamAttitude::Hostile;
}
} // namespace Teams

namespace AbilitySystem
{
void SendGameplayEventToInstigator(AActor* Instigator, AActor* Target, FGameplayTag Tag, float Magnitude)
{
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, MoveTemp(Tag), MakePayload(Instigator, Target, Magnitude));
}

void SendGameplayEventToTarget(AActor* Instigator, AActor* Target, FGameplayTag Tag, float Magnitude)
{
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, MoveTemp(Tag), MakePayload(Instigator, Target, Magnitude));
}

void EndTask(UAbilityTask* Task)
{
    if (IsValid(Task))
    {
        Task->EndTask();
    }
}
} // namespace AbilitySystem
