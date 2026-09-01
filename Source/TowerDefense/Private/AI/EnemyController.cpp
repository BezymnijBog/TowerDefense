// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/EnemyController.h"

#include "AI/EnvironmentQuery/EnvQueryItemType_TargetCells.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WayComponent.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Subsystems/WorldGridSubsystem.h"

void AEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    if (!Result.IsSuccess())
    {
        return;
    }
    SelectNextTarget();
}

void AEnemyController::OnDeath()
{
    GetWorld()->GetSubsystem<UWorldGridSubsystem>()->FreeCell(ActiveCell);
    Super::OnDeath();
}

void AEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (IsValid(InPawn) && IsValid(BehaviorTree))
    {
        UBlackboardComponent* const BlackboardComponent = GetBlackboardComponent();
        BlackboardComponent->SetValueAsVector(BlackboardKeys.DestinationPoint, SelectNextPatrolPoint());
    }
}

void AEnemyController::SelectNextTarget()
{
    if (HasValidTarget())
    {
        return;
    }

    TArray<AActor*> VisibleActors;
    RemoveCurrentAttackTarget();
    if (ActiveCell)
    {
        if (const TWeakObjectPtr<AActor>* const Target = ActiveCell->PossibleTargets.FindArbitraryElement(); Target && Target->IsValid())
        {
            SetCurrentAttackTarget(Target->Get());
        }
        else
        {
            GetWorld()->GetSubsystem<UWorldGridSubsystem>()->FreeCell(ActiveCell);
            ActiveCell = nullptr;
            GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::SelectNextTarget);
        }
    }
    else if (PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), VisibleActors); VisibleActors.IsEmpty())
    {
        GetBlackboardComponent()->SetValueAsVector(BlackboardKeys.DestinationPoint, SelectNextPatrolPoint());
    }
    else
    {
        RunEnvironmentQuery();
    }
}

FVector AEnemyController::SelectNextPatrolPoint()
{
    const UWayComponent* const WayComponent = UInterfaceFunctionLibrary::GetWayComponent(GetPawn());
    if (!IsValid(WayComponent))
    {
        return FVector::ZeroVector;
    }

    if (bIsOffensive)
    {
        bIsOffensive = false;
        return WayComponent->GetClosestPoint();
    }

    return WayComponent->GetNextTargetPoint();
}

void AEnemyController::OnQueryComplete(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> EnvQueryResult)
{
    if (!EnvQueryResult.IsValid() || !EnvQueryResult->IsSuccessful())
    {
        SelectNextTarget();
        return;
    }

    ActiveCell = EnvQueryResult->GetItemAsTypeChecked<UEnvQueryItemType_TargetCells>(0);
    if (ActiveCell && GetWorld()->GetSubsystem<UWorldGridSubsystem>()->TryReserveCell(ActiveCell, this))
    {
        GetBlackboardComponent()->SetValueAsVector(BlackboardKeys.DestinationPoint, ActiveCell->WorldCoordinate);
        if (const TWeakObjectPtr<AActor>* const Target = ActiveCell->PossibleTargets.FindArbitraryElement(); Target && Target->IsValid())
        {
            SetCurrentAttackTarget(Target->Get());
        }
    }
}

void AEnemyController::RunEnvironmentQuery()
{
    if (!IsValid(SearchCellQuery))
    {
        UE_LOG(LogEQS, Error, TEXT("Invalid EnvQuery asset for %s"), *GetName());
        return;
    }

    FEnvQueryRequest QueryRequest(SearchCellQuery, this);
    QueryRequest.SetFloatParam(TEXT("TargetCells.SearchRadius"), PerceptionComponent->GetSenseConfig<UAISenseConfig_Sight>()->SightRadius);
    QueryRequest.Execute(EEnvQueryRunMode::RandomBest25Pct, this, &AEnemyController::OnQueryComplete);
}
