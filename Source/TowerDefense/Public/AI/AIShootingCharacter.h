// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Interfaces/ShooterInterface.h"
#include "AIShootingCharacter.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API AAIShootingCharacter : public ATowerDefenseAICharacter, public IShooterInterface
{
    GENERATED_BODY()

public:
    AAIShootingCharacter();

    virtual AProjectile* ActivateProjectile() override;
    virtual void LaunchActiveProjectile(const AActor* Target) override;;

protected:
    void OnProjectileHit(AProjectile* Projectile);

    UPROPERTY(VisibleAnywhere, Category = Components)
    TObjectPtr<UStaticMeshComponent> ProjectileMesh;

    UPROPERTY(EditDefaultsOnly, Category = Shooting)
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(VisibleInstanceOnly, Category = Shooting)
    TArray<AProjectile*> ReserveProjectiles;

    UPROPERTY(VisibleInstanceOnly, Category = Shooting)
    TObjectPtr<AProjectile> ActiveProjectile;
};
