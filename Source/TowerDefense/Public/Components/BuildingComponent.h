// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "BuildingComponent.generated.h"

DECLARE_DELEGATE_OneParam(FCanBuildChanged, bool)

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOWERDEFENSE_API UBuildingComponent : public UBoxComponent
{
    GENERATED_BODY()

public:
    FCanBuildChanged CanBuildChanged;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleInstanceOnly)
    TSet<UBuildingComponent*> OverlapComponents;

    UPROPERTY(VisibleInstanceOnly)
    bool bCanBuild = true;

private:
    void SetCanBuild(bool bValue);

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                        AActor* OtherActor,
                        UPrimitiveComponent* OtherComp,
                        int OtherBodyIndex,
                        bool bFromSweep,
                        const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);
};
