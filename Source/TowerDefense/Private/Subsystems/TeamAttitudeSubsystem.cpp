// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru


#include "Subsystems/TeamAttitudeSubsystem.h"

#include "GenericTeamAgentInterface.h"
#include "Utils/BaseUtils.h"

void UTeamAttitudeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    FGenericTeamId::SetAttitudeSolver(Teams::AttitudeSolveFunction);
}

void UTeamAttitudeSubsystem::Deinitialize()
{
    Super::Deinitialize();
    FGenericTeamId::ResetAttitudeSolver();
}
