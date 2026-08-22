// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/TowerDefenseAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Algo/RemoveIf.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WayComponent.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Damage.h"

ATowerDefenseAIController::ATowerDefenseAIController()
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
}

void ATowerDefenseAIController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    Super::ActorsPerceptionUpdated(UpdatedActors);
    SelectNextTarget();
}

void ATowerDefenseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    if (!Result.IsSuccess())
    {
        return;
    }
    SelectNextTarget();
}

UAbilitySystemComponent* ATowerDefenseAIController::GetAbilitySystemComponent() const
{
    return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
}

bool ATowerDefenseAIController::IsDead() const
{
    return UInterfaceFunctionLibrary::IsActorDead(GetPawn());
}

void ATowerDefenseAIController::OnDeath()
{
    BrainComponent->StopLogic(TEXT("Is dead"));
    Blackboard->SetValueAsFloat(BlackboardKeys.AttackRange, 0.f);
    Blackboard->SetValueAsObject(BlackboardKeys.AttackTarget, nullptr);
    Blackboard->SetValueAsVector(BlackboardKeys.DestinationPoint, FVector::ZeroVector);
    StopMovement();
    UnPossess();
    Destroy();
}

FDeathDelegate& ATowerDefenseAIController::GetDeathDelegate()
{
    return UInterfaceFunctionLibrary::GetDeathDelegate(GetPawn());
}

AActor* ATowerDefenseAIController::GetClosestSensedActor() const
{
    TArray<AActor*> VisibleActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), VisibleActors);
    if (VisibleActors.IsEmpty())
    {
        return nullptr;
    }

    const APawn* const ControlledPawn = GetPawn();
    if (!IsValid(ControlledPawn))
    {
        return VisibleActors[0];
    }

    VisibleActors.SetNum(Algo::RemoveIf(VisibleActors, [](const AActor* Actor) { return UInterfaceFunctionLibrary::IsActorDead(Actor); }));
    const FVector PawnLocation = ControlledPawn->GetActorLocation();
    if (IsBeingBeaten())
    {
        VisibleActors.SetNum(Algo::RemoveIf(VisibleActors, [](const AActor* Actor) { return !Actor->IsA<APawn>(); }));
    }

    return VisibleActors.IsEmpty() ?
        nullptr :
        *Algo::MinElementBy(VisibleActors, [&PawnLocation](const AActor* Actor) { return (Actor->GetActorLocation() - PawnLocation).SquaredLength(); });
}

FVector ATowerDefenseAIController::SelectNextPatrolPoint()
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

void ATowerDefenseAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (IsValid(InPawn) && IsValid(BehaviorTree))
    {
        SetGenericTeamId(FGenericTeamId::GetTeamIdentifier(InPawn));
        RunBehaviorTree(BehaviorTree);
        UBlackboardComponent* const BlackboardComponent = GetBlackboardComponent();
        BlackboardComponent->SetValueAsFloat(BlackboardKeys.AttackRange, UInterfaceFunctionLibrary::GetAttackRange(InPawn));
        BlackboardComponent->SetValueAsVector(BlackboardKeys.DestinationPoint, SelectNextPatrolPoint());
        PerceptionComponent->RequestStimuliListenerUpdate();
        SelectNextTarget();
    }
}

bool ATowerDefenseAIController::IsBeingBeaten() const
{
    TArray<AActor*> DamagingActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), DamagingActors);
    return !DamagingActors.IsEmpty();
}

void ATowerDefenseAIController::SelectNextTarget()
{
    if (HasValidTarget())
    {
        return;
    }

    RemoveCurrentAttackTarget();
    if (AActor* const Target = GetClosestSensedActor(); IsValid(Target))
    {
        SetCurrentAttackTarget(Target);
    }
    else
    {
        GetBlackboardComponent()->SetValueAsVector(BlackboardKeys.DestinationPoint, SelectNextPatrolPoint());
    }
}

void ATowerDefenseAIController::RemoveCurrentAttackTarget()
{
    UBlackboardComponent* const BlackboardComponent = GetBlackboardComponent();
    if (AActor* const CurrentTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(BlackboardKeys.AttackTarget)); IsValid(CurrentTarget))
    {
        UInterfaceFunctionLibrary::GetDeathDelegate(CurrentTarget).Remove(TargetDeathHandle);
        BlackboardComponent->SetValueAsObject(BlackboardKeys.AttackTarget, nullptr);
        TargetDeathHandle.Reset();
    }
}

void ATowerDefenseAIController::SetCurrentAttackTarget(AActor* NewTarget)
{
    bIsOffensive = true;
    GetBlackboardComponent()->SetValueAsObject(BlackboardKeys.AttackTarget, NewTarget);
    TargetDeathHandle = UInterfaceFunctionLibrary::GetDeathDelegate(NewTarget).AddUObject(this, &ThisClass::SelectNextTarget);
}

bool ATowerDefenseAIController::HasValidTarget() const
{
    const UBlackboardComponent* const BlackboardComponent = GetBlackboardComponent();
    if (const AActor* const CurrentTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(BlackboardKeys.AttackTarget)); IsValid(CurrentTarget))
    {
        return !UInterfaceFunctionLibrary::IsActorDead(CurrentTarget);
    }
    return false;
}
