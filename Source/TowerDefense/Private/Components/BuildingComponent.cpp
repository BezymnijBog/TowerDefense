// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/BuildingComponent.h"

void UBuildingComponent::BeginPlay()
{
    Super::BeginPlay();

    OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
    OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

void UBuildingComponent::SetCanBuild(bool bValue)
{
    if (bCanBuild != bValue)
    {
        bCanBuild = bValue;
        std::ignore = CanBuildChanged.ExecuteIfBound(bCanBuild);
    }
}

void UBuildingComponent::OnBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int, bool bFromSweep, const FHitResult&)
{
    UBuildingComponent* const OtherBuildingComponent = OtherActor->GetComponentByClass<ThisClass>();
    if (!IsValid(OtherBuildingComponent) || OtherBuildingComponent == this)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("%s overlapped %s; bFromSweep=%d"), *GetOwner()->GetName(), *OtherActor->GetName(), bFromSweep);
    OtherBuildingComponent->OverlapComponents.Add(OtherBuildingComponent);
    OtherBuildingComponent->SetCanBuild(OtherBuildingComponent->OverlapComponents.IsEmpty());
}

void UBuildingComponent::OnEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int)
{
    UBuildingComponent* const OtherBuildingComponent = OtherActor->GetComponentByClass<ThisClass>();
    if (!IsValid(OtherBuildingComponent) || OtherBuildingComponent == this)
    {
        return;
    }

    OtherBuildingComponent->OverlapComponents.Remove(OtherBuildingComponent);
    OtherBuildingComponent->SetCanBuild(OtherBuildingComponent->OverlapComponents.IsEmpty());
}
