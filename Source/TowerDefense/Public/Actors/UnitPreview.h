// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitPreview.generated.h"

class UCapsuleComponent;

UCLASS()
class TOWERDEFENSE_API AUnitPreview : public AActor
{
    GENERATED_BODY()

public:
    AUnitPreview();

    bool HasCollisionIntersections() const;

    USkeletalMeshComponent* GetMesh() const;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = Components)
    TObjectPtr<UCapsuleComponent> CapsuleCollision;

    UPROPERTY(VisibleAnywhere, Category = Components)
    TObjectPtr<USkeletalMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, Category = Components)
    TSet<AActor*> OverlappedActors;

private:
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
