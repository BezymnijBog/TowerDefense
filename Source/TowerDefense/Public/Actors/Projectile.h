// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "Utils/BaseUtils.h"
#include "Projectile.generated.h"

class UNiagaraComponent;
class USphereComponent;
class UProjectileMovementComponent;

DECLARE_DELEGATE_OneParam(FOnHit, class AProjectile*)

UCLASS()
class TOWERDEFENSE_API AProjectile : public AActor, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    AProjectile();

    void Activate(const FTransform& Transform, const AActor* Target, AActor* InInstigator);
    void Deactivate();

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

    FOnHit OnHitDelegate;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USphereComponent> SphereRoot;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UNiagaraComponent> VisualEffect;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UProjectileMovementComponent> MovementComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    FGenericTeamId TeamId = Teams::TeamPlayer;

    UPROPERTY(VisibleInstanceOnly, Category = "Projectile")
    TWeakObjectPtr<AActor> ShotInstigator;

private:
    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* OverlappedComponent,
                         AActor* OtherActor,
                         UPrimitiveComponent* OtherComp,
                         int OtherBodyIndex,
                         bool bFromSweep,
                         const FHitResult& SweepResult);
};
