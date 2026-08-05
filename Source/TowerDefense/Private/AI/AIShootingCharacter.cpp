// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/AIShootingCharacter.h"

#include "Actors/Projectile.h"

AAIShootingCharacter::AAIShootingCharacter()
{
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(GetMesh(), TEXT("Weapon_Socket_R"));
}

AProjectile* AAIShootingCharacter::ActivateProjectile()
{
    AProjectile* const Projectile = ReserveProjectiles.IsEmpty() ? GetWorld()->SpawnActor<AProjectile>(ProjectileClass) : ReserveProjectiles.Pop();
    ActiveProjectile = Projectile;
    return Projectile;
}

void AAIShootingCharacter::LaunchActiveProjectile(const AActor* Target)
{
    ActiveProjectile->Activate(Target, this);
    ActiveProjectile->OnHitDelegate.BindUObject(this, &ThisClass::OnProjectileHit);
    ActiveProjectile = nullptr;
}

void AAIShootingCharacter::OnProjectileHit(AProjectile* Projectile)
{
    Projectile->Deactivate();
    Projectile->OnHitDelegate.Unbind();
    Projectile->SetActorTransform(FTransform::Identity);
    ReserveProjectiles.Emplace(Projectile);
}
