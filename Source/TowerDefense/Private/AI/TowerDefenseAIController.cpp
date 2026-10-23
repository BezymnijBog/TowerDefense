// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/TowerDefenseAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Algo/RemoveIf.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Sight.h"

ATowerDefenseAIController::ATowerDefenseAIController()
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
}

void ATowerDefenseAIController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    Super::ActorsPerceptionUpdated(UpdatedActors);
    TArray<AActor*> VisibleActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), VisibleActors);
    if (const AActor* Target = GetCurrentTarget(); IsValid(Target) && !VisibleActors.Contains(Target))
    {
        RemoveCurrentAttackTarget();
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
    GetWorldTimerManager().ClearTimer(PositionRefreshTimer);
    RemoveCurrentAttackTarget();
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

float ATowerDefenseAIController::GetAttackRange() const
{
    return UInterfaceFunctionLibrary::GetAttackRange(GetPawn());
}

int32 ATowerDefenseAIController::GetAttackRadiusInCells() const
{
    return UInterfaceFunctionLibrary::GetAttackRadiusInCells(GetPawn());
}

EUnitAttackPattern ATowerDefenseAIController::GetAttackPattern() const
{
    return UInterfaceFunctionLibrary::GetAttackPattern(GetPawn());
}

bool ATowerDefenseAIController::CanAttackFrom(const FVector& Position, const AActor* Target) const
{
    return UInterfaceFunctionLibrary::CanAttackFrom(GetPawn(), Position, Target);
}

bool ATowerDefenseAIController::CanAttack(const AActor* Target) const
{
    return UInterfaceFunctionLibrary::CanAttack(GetPawn(), Target);
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

    const ATowerDefenseAICharacter* CombatCharacter = Cast<ATowerDefenseAICharacter>(ControlledPawn);
    VisibleActors.SetNum(
        Algo::RemoveIf(VisibleActors, [CombatCharacter](const AActor* Actor) { return !IsValid(CombatCharacter) || !CombatCharacter->CanAttack(Actor); }));
    const FVector PawnLocation = ControlledPawn->GetActorLocation();
    if (IsBeingBeaten())
    {
        VisibleActors.SetNum(Algo::RemoveIf(VisibleActors, [](const AActor* Actor) { return !Actor->IsA<APawn>(); }));
    }

    return VisibleActors.IsEmpty() ?
        nullptr :
        *Algo::MinElementBy(VisibleActors, [&PawnLocation](const AActor* Actor) { return (Actor->GetActorLocation() - PawnLocation).SquaredLength(); });
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
        PerceptionComponent->RequestStimuliListenerUpdate();
        SelectNextTarget();
        GetWorldTimerManager().SetTimer(PositionRefreshTimer, this, &ThisClass::SelectNextTarget, PositionRefreshInterval, true);
    }
}

void ATowerDefenseAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(PositionRefreshTimer);
    RemoveCurrentAttackTarget();
    Super::EndPlay(EndPlayReason);
}

void ATowerDefenseAIController::SelectNextTarget()
{
    if (!IsValid(GetPawn()) || IsDead() || !IsValid(GetBlackboardComponent()))
    {
        return;
    }
    if (CanAttackCurrentTarget())
    {
        return;
    }

    RemoveCurrentAttackTarget();
    if (AActor* const Target = GetClosestSensedActor(); IsValid(Target))
    {
        SetCurrentAttackTarget(Target);
    }
}

bool ATowerDefenseAIController::IsBeingBeaten() const
{
    TArray<AActor*> DamagingActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), DamagingActors);
    return !DamagingActors.IsEmpty();
}

void ATowerDefenseAIController::RemoveCurrentAttackTarget()
{
    UBlackboardComponent* const BlackboardComponent = GetBlackboardComponent();
    if (!IsValid(BlackboardComponent))
    {
        return;
    }
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
    if (const AActor* const CurrentTarget = GetCurrentTarget(); IsValid(CurrentTarget))
    {
        return !UInterfaceFunctionLibrary::IsActorDead(CurrentTarget);
    }
    return false;
}

AActor* ATowerDefenseAIController::GetCurrentTarget() const
{
    const UBlackboardComponent* const BlackboardComponent = GetBlackboardComponent();
    return IsValid(BlackboardComponent) ? Cast<AActor>(BlackboardComponent->GetValueAsObject(BlackboardKeys.AttackTarget)) : nullptr;
}

bool ATowerDefenseAIController::CanAttackCurrentTarget() const
{
    return CanAttack(GetCurrentTarget());
}
