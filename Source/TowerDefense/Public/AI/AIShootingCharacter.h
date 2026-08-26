// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "DefenderCharacter.h"
#include "Interfaces/ShooterInterface.h"
#include "AIShootingCharacter.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API AAIShootingCharacter : public ADefenderCharacter, public IShooterInterface
{
    GENERATED_BODY()

public:
    AAIShootingCharacter(const FObjectInitializer& ObjectInitializer);

    virtual void ActivateProjectile() override;
    virtual void LaunchActiveProjectile() override;

protected:
    void OnProjectileHit(AProjectile* Projectile);

    UPROPERTY(VisibleAnywhere, Category = Components)
    TObjectPtr<UStaticMeshComponent> ProjectileMesh;

    UPROPERTY(EditDefaultsOnly, Category = Shooting)
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category = Shooting)
    FName TargetBlackboardKey;

    UPROPERTY(EditDefaultsOnly, Category = Shooting)
    FName SpawnSocketName;

    UPROPERTY(EditDefaultsOnly, Category = Shooting)
    FRotator ReleaseRotation;

    UPROPERTY(VisibleInstanceOnly, Category = Shooting)
    TArray<AProjectile*> ReserveProjectiles;

    UPROPERTY(VisibleInstanceOnly, Category = Shooting)
    TWeakObjectPtr<AProjectile> ActiveProjectile;
};
