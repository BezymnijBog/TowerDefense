// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/EnvironmentQuery/EnvQueryGenerator_TargetSlots.h"

#include "AI/EnvironmentQuery/EnvQueryItemType_AttackSlot.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Internationalization/Internationalization.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionListenerInterface.h"
#include "Perception/AISense.h"

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

UEnvQueryGenerator_TargetSlots::UEnvQueryGenerator_TargetSlots(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    ItemType = UEnvQueryItemType_Point::StaticClass();

    SearchRadius.DefaultValue = -1.f;
    ListenerContext = UEnvQueryContext_Querier::StaticClass();
}

void UEnvQueryGenerator_TargetSlots::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
    QueryInstance.ValueSize = GetDefault<UEnvQueryItemType_AttackSlot>()->GetValueSize();
    TArray<FAttackSlot> AttackSlots;
    for (const AActor* const PerceivedActor : GetPerceivedActors(QueryInstance))
    {
        AttackSlots.Append(UInterfaceFunctionLibrary::GetAttackSlots(PerceivedActor));
    }
    QueryInstance.AddItemData<UEnvQueryItemType_AttackSlot>(AttackSlots);
}

FText UEnvQueryGenerator_TargetSlots::GetDescriptionTitle() const
{
    FFormatNamedArguments Args;
    Args.Add(TEXT("DescribeContext"), UEnvQueryTypes::DescribeContext(ListenerContext));
    return FText::Format(LOCTEXT("DescriptionGenerateTargetSlots", "Attack slots perceived by {DescribeContext}"), Args);
}

FText UEnvQueryGenerator_TargetSlots::GetDescriptionDetails() const
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

TArray<AActor*> UEnvQueryGenerator_TargetSlots::GetListeners(FEnvQueryInstance& QueryInstance) const
{
    TArray<AActor*> Listeners;
    QueryInstance.PrepareContext(ListenerContext, Listeners);
    return Listeners;
}

TSet<AActor*> UEnvQueryGenerator_TargetSlots::GetPerceivedActors(FEnvQueryInstance& QueryInstance) const
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
            if (PerceivedActor && (RadiusValue <= 0 || FVector::DistSquared(ListenerLocation, PerceivedActor->GetActorLocation()) < RadiusSq)
                && (!AllowedActorClass || PerceivedActor->IsA(AllowedActorClass.Get())))
            {
                Result.Add(PerceivedActor);
            }
        }
    }

    return Result;
}

#undef LOCTEXT_NAMESPACE
