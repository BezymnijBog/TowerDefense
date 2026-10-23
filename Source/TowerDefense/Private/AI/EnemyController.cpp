// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/EnemyController.h"
#include "AI/TowerDefenseAICharacter.h"
#include "AI/EnvironmentQuery/EnvQueryItemType_TargetCells.h"
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
#include "Utils/BaseUtils.h"

void AEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    if (Result.Code == EPathFollowingResult::Aborted)
    {
        return;
    }
    if (!Result.IsSuccess())
    {
        FailedCell = ActiveCell;
        FailedCellRetryTime = GetWorld()->GetTimeSeconds() + PositionRefreshInterval * 4.f;
        ReleaseCombatCell();
        RemoveCurrentAttackTarget();
    }
    SelectNextTarget();
}

void AEnemyController::ReleaseCombatCell()
{
    if (UWorldGridSubsystem* Grid = Utils::GetWorldSubsystem<UWorldGridSubsystem>(this); IsValid(Grid))
    {
        if (ActiveCell && ActiveCell->ReservedBy == this)
        {
            Grid->FreeCell(ActiveCell);
        }
    }
    ActiveCell = nullptr;
}

void AEnemyController::OnDeath()
{
    GetWorldTimerManager().ClearTimer(PositionRefreshTimer);
    ReleaseCombatCell();
    RemoveCurrentAttackTarget();
    Super::OnDeath();
}

void AEnemyController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(PositionRefreshTimer);
    if (PendingQueryId != INDEX_NONE)
    {
        if (UEnvQueryManager* Manager = UEnvQueryManager::GetCurrent(GetWorld()); IsValid(Manager))
        {
            const int32 QueryId = PendingQueryId;
            PendingQueryId = INDEX_NONE;
            Manager->AbortQuery(QueryId);
        }
    }
    ReleaseCombatCell();
    RemoveCurrentAttackTarget();
    Super::EndPlay(EndPlayReason);
}

AActor* AEnemyController::FindTargetFrom(const FVector& Position) const
{
    const ATowerDefenseAICharacter* CombatCharacter = Cast<ATowerDefenseAICharacter>(GetPawn());
    if (!IsValid(CombatCharacter) || !IsValid(PerceptionComponent))
    {
        return nullptr;
    }
    TArray<AActor*> VisibleActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), VisibleActors);
    AActor* BestTarget = nullptr;
    double BestDistance = TNumericLimits<double>::Max();
    for (AActor* Target : VisibleActors)
    {
        if (CombatCharacter->CanAttackFrom(Position, Target))
        {
            const double Distance = FVector::DistSquared2D(Position, Target->GetActorLocation());
            if (Distance < BestDistance)
            {
                BestTarget = Target;
                BestDistance = Distance;
            }
        }
    }
    return BestTarget;
}

void AEnemyController::SelectNextTarget()
{
    const ATowerDefenseAICharacter* CombatCharacter = Cast<ATowerDefenseAICharacter>(GetPawn());
    UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
    if (!IsValid(CombatCharacter) || CombatCharacter->IsDead() || !IsValid(BlackboardComponent))
    {
        return;
    }
    if (ActiveCell && ActiveCell->ReservedBy == this && !ActiveCell->Occupant.IsValid())
    {
        if (HasValidTarget() && CombatCharacter->CanAttackFrom(ActiveCell->WorldCoordinate, GetCurrentTarget()))
        {
            return;
        }
        RemoveCurrentAttackTarget();
        if (AActor* Target = FindTargetFrom(ActiveCell->WorldCoordinate); IsValid(Target))
        {
            SetCurrentAttackTarget(Target);
            return;
        }
    }
    ReleaseCombatCell();
    RemoveCurrentAttackTarget();
    if (PendingQueryId != INDEX_NONE)
    {
        return;
    }
    TArray<AActor*> VisibleActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), VisibleActors);
    const bool bHasEnemies = VisibleActors.ContainsByPredicate([CombatCharacter](const AActor* Actor)
    {
        return IsValid(Actor) && !UInterfaceFunctionLibrary::IsActorDead(Actor)
            && FGenericTeamId::GetAttitude(CombatCharacter, Actor) == ETeamAttitude::Hostile;
    });
    if (bHasEnemies)
    {
        // Hold position when all combat cells are reserved instead of pushing into the front.
        BlackboardComponent->SetValueAsVector(BlackboardKeys.DestinationPoint, CombatCharacter->GetActorLocation());
        RunEnvironmentQuery();
    }
    else if (GetMoveStatus() != EPathFollowingStatus::Moving)
    {
        BlackboardComponent->SetValueAsVector(BlackboardKeys.DestinationPoint, SelectNextPatrolPoint());
    }
}

FVector AEnemyController::SelectNextPatrolPoint()
{
    const UWayComponent* WayComponent = UInterfaceFunctionLibrary::GetWayComponent(GetPawn());
    if (!IsValid(WayComponent))
    {
        return IsValid(GetPawn()) ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
    }
    if (bIsOffensive)
    {
        bIsOffensive = false;
        return WayComponent->GetClosestPoint();
    }
    return WayComponent->GetNextTargetPoint();
}

void AEnemyController::OnQueryComplete(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> Result)
{
    if (!Result.IsValid() || Result->QueryID != PendingQueryId)
    {
        return;
    }
    PendingQueryId = INDEX_NONE;
    UWorldGridSubsystem* Grid = Utils::GetWorldSubsystem<UWorldGridSubsystem>(this);
    if (!Result->IsSuccessful() || !IsValid(Grid) || !IsValid(GetPawn()) || IsDead())
    {
        return;
    }
    for (int32 Index = 0; Index < Result->Items.Num(); ++Index)
    {
        const FGridCellInfo* Cell = Result->GetItemAsTypeChecked<UEnvQueryItemType_TargetCells>(Index);
        if (!Cell || (Cell == FailedCell && GetWorld()->GetTimeSeconds() < FailedCellRetryTime))
        {
            continue;
        }
        AActor* Target = FindTargetFrom(Cell->WorldCoordinate);
        if (IsValid(Target) && Grid->TryReserveCell(Cell, this))
        {
            ActiveCell = Cell;
            GetBlackboardComponent()->SetValueAsVector(BlackboardKeys.DestinationPoint, Cell->WorldCoordinate);
            SetCurrentAttackTarget(Target);
            return;
        }
    }
}

void AEnemyController::RunEnvironmentQuery()
{
    if (!IsValid(SearchCellQuery) || PendingQueryId != INDEX_NONE)
    {
        return;
    }
    FEnvQueryRequest Request(SearchCellQuery, this);
    const UAISenseConfig_Sight* Sight = PerceptionComponent->GetSenseConfig<UAISenseConfig_Sight>();
    Request.SetFloatParam(TEXT("TargetCells.SearchRadius"), IsValid(Sight) ? Sight->SightRadius : -1.f);
    PendingQueryId = Request.Execute(EEnvQueryRunMode::AllMatching, this, &ThisClass::OnQueryComplete);
}
