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
namespace RollUtils
{
int32 RollDice(int32 N)
{
    int32 Result = 0;
    for (int32 I = 0; I < N; ++I)
    {
        static constexpr int32 DiceMin = 1;
        static constexpr int32 DiceMax = 6;
        Result += FMath::RandRange(DiceMin, DiceMax);
    }
    return Result;
}

ERollResult CheckSuccessMargin(int32& Margin, int32 Difficulty)
{
    const int32 RollResult = RollDice(3);
    Margin = RollResult - Difficulty;
    if (RollResult == 18 || (RollResult == 17 && Difficulty <= 16) || Margin >= 10)
    {
        return CriticalFailure;
    }

    if (RollResult == 3 || (RollResult == 4 && Difficulty >= 5) || (Margin <= -10 && RollResult <= 6))
    {
        return CriticalSuccess;
    }

    return Margin > 0 ? Failure : Success;
}

bool IsCheckSuccessful(int32 Difficulty)
{
    switch (int32 Margin = 0; CheckSuccessMargin(Margin, Difficulty))
    {
        case CriticalSuccess:
        case Success:
            return true;
        case Failure:
        case CriticalFailure:
        default:
            return false;
    }
}
} // namespace RollUtils
