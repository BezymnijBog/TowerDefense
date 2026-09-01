// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/EnvironmentQuery/EnvQueryGenerator_TargetCells.h"

#include "AI/EnvironmentQuery/EnvQueryItemType_TargetCells.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Internationalization/Internationalization.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionListenerInterface.h"
#include "Perception/AISense.h"
#include "Subsystems/WorldGridSubsystem.h"

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"

namespace
{
UAIPerceptionComponent* GetPerceptionComponent(AActor* Listener)
{
    if (Listener->Implements<UAIPerceptionListenerInterface>())
    {
        return UInterfaceFunctionLibrary::GetPerceptionComponent(Listener);
    }

    if (const APawn* AsPawn = Cast<APawn>(Listener); IsValid(AsPawn))
    {
        return UInterfaceFunctionLibrary::GetPerceptionComponent(AsPawn->GetController());
    }

    return nullptr;
}
} // namespace

UEnvQueryGenerator_TargetCells::UEnvQueryGenerator_TargetCells(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    ItemType = UEnvQueryItemType_TargetCells::StaticClass();

    SearchRadius.DefaultValue = -1.f;
    ListenerContext = UEnvQueryContext_Querier::StaticClass();
}

void UEnvQueryGenerator_TargetCells::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
    //QueryInstance.ValueSize = GetDefault<UEnvQueryItemType_TargetCells>()->GetValueSize();
    TSet<FIntVector2> AttackSlots;
    for (const AActor* const PerceivedActor : GetPerceivedActors(QueryInstance))
    {
        AttackSlots.Append(UInterfaceFunctionLibrary::GetAdjacentCells(PerceivedActor));
    }

    const UWorld* const World = GetWorld();
    if (!IsValid(World))
    {
        UE_LOG(LogEQS, Error, TEXT("Can't get world subsystem, world is invalid"));
        return;
    }

    const UWorldGridSubsystem* const Subsystem = World->GetSubsystem<UWorldGridSubsystem>();
    for (const FIntVector2& CellCoord : AttackSlots)
    {
        if (const FGridCellInfo* const Info = Subsystem->GetCellInfo(CellCoord); Info)
        {
            QueryInstance.AddItemData<UEnvQueryItemType_TargetCells>(Info);
        }
    }
}

FText UEnvQueryGenerator_TargetCells::GetDescriptionTitle() const
{
    FFormatNamedArguments Args;
    Args.Add(TEXT("DescribeContext"), UEnvQueryTypes::DescribeContext(ListenerContext));
    return FText::Format(LOCTEXT("DescriptionGenerateTargetSlots", "Grid cells perceived by {DescribeContext}"), Args);
}

FText UEnvQueryGenerator_TargetCells::GetDescriptionDetails() const
{
    FFormatNamedArguments Args;
    if (SearchRadius.IsDynamic() == false && SearchRadius.GetValue() <= 0.f)
    {
        Args.Add(TEXT("Radius"), FText::FromString(TEXT("Perception Range")));
    }
    else
    {
        Args.Add(TEXT("Radius"), FText::FromString(SearchRadius.ToString()));
    }

    if (SenseToUse)
    {
        Args.Add(TEXT("Sense"), FText::FromString(SenseToUse->GetName()));
    }
    else
    {
        Args.Add(TEXT("Sense"), FText::FromString(TEXT("All senses")));
    }

    if (AllowedActorClass)
    {
        Args.Add(TEXT("ActorClass"), FText::FromString(AllowedActorClass->GetName()));
        return FText::Format(LOCTEXT("PerceivedActorsOfClassDescription", "radius: {Radius}\nsense: {Sense}\nactors of class: {ActorClass}"), Args);
    }

    return FText::Format(LOCTEXT("PerceivedActorsDescription", "radius: {Radius}\nsense: {Sense}"), Args);
}

TArray<AActor*> UEnvQueryGenerator_TargetCells::GetListeners(FEnvQueryInstance& QueryInstance) const
{
    TArray<AActor*> Listeners;
    QueryInstance.PrepareContext(ListenerContext, Listeners);
    return Listeners;
}

TSet<AActor*> UEnvQueryGenerator_TargetCells::GetPerceivedActors(FEnvQueryInstance& QueryInstance) const
{
    const UObject* QueryOwner = QueryInstance.Owner.Get();
    if (!IsValid(QueryOwner))
    {
        return {};
    }

    TSet<AActor*> Result;

    SearchRadius.BindData(QueryOwner, QueryInstance.QueryID);
    const float RadiusValue = SearchRadius.GetValue();
    const float RadiusSq = FMath::Square(RadiusValue);

    for (AActor* const ListenerActor : GetListeners(QueryInstance))
    {
        const UAIPerceptionComponent* PerceptionComponent = GetPerceptionComponent(ListenerActor);
        if (!IsValid(PerceptionComponent))
        {
            UE_VLOG(QueryOwner,
                    LogEQS,
                    Error,
                    TEXT("Tried to use EnvQueryGenerator_TargetSlots while query context actor\'s %s UAIPerceptionComponent is missing"),
                    *ListenerActor->GetName());
            continue;
        }

        const FVector ListenerLocation = ListenerActor->GetActorLocation();
        TArray<AActor*> LocalPerceivedActors;

        PerceptionComponent->GetCurrentlyPerceivedActors(SenseToUse, LocalPerceivedActors);

        for (AActor* const PerceivedActor : LocalPerceivedActors)
        {
            if (IsValidPerceivedActor(PerceivedActor)
                && (RadiusValue <= 0 || FVector::DistSquared(ListenerLocation, PerceivedActor->GetActorLocation()) < RadiusSq))
            {
                Result.Add(PerceivedActor);
            }
        }
    }

    return Result;
}

bool UEnvQueryGenerator_TargetCells::IsValidPerceivedActor(const AActor* Actor) const
{
    return IsValid(Actor) && (!AllowedActorClass || Actor->IsA(AllowedActorClass.Get())) && !UInterfaceFunctionLibrary::IsActorDead(Actor);
}

#undef LOCTEXT_NAMESPACE
