// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/AIShootingCharacter.h"

#include "Actors/Projectile.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIShootingCharacter::AAIShootingCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(GetMesh(), TEXT("Weapon_Socket_R"));
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAIShootingCharacter::ActivateProjectile()
{
    if (ActiveProjectile.IsValid())
    {
        OnProjectileHit(ActiveProjectile.Get());
    }
    if (ReserveProjectiles.IsEmpty())
    {
        FActorSpawnParameters Parameters;
        Parameters.Owner = this;
        Parameters.Instigator = this;
        ActiveProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Parameters);
        if (ActiveProjectile.IsValid())
        {
            OwnedProjectiles.Add(ActiveProjectile.Get());
        }
    }
    else
    {
        ActiveProjectile = ReserveProjectiles.Pop();
    }
    ProjectileMesh->SetVisibility(ActiveProjectile.IsValid());
}

void AAIShootingCharacter::LaunchActiveProjectile()
{
    AAIController* const CurrentController = GetInstigatorController<AAIController>();
    if (!IsValid(CurrentController) || !ActiveProjectile.IsValid())
    {
        return;
    }

    const AActor* const Target = Cast<AActor>(CurrentController->GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKey));
    if (!CanAttack(Target))
    {
        OnProjectileHit(ActiveProjectile.Get());
        ActiveProjectile = nullptr;
        ProjectileMesh->SetVisibility(false);
        return;
    }

    ProjectileMesh->SetVisibility(false);

    ActiveProjectile->SetActorLocation(GetMesh()->GetSocketLocation(SpawnSocketName));
    ActiveProjectile->SetActorRotation((Target->GetActorLocation() - ActiveProjectile->GetActorLocation()).Rotation());
    AProjectile* Projectile = ActiveProjectile.Get();
    ActiveProjectile = nullptr;
    Projectile->OnHitDelegate.BindUObject(this, &ThisClass::OnProjectileHit);
    Projectile->Activate(Target, this);
}

void AAIShootingCharacter::OnProjectileHit(AProjectile* Projectile)
{
    Projectile->Deactivate();
    Projectile->OnHitDelegate.Unbind();
    Projectile->SetActorTransform(FTransform::Identity);
    ReserveProjectiles.Emplace(Projectile);
}

void AAIShootingCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    for (AProjectile* Projectile : OwnedProjectiles)
    {
        if (IsValid(Projectile))
        {
            Projectile->OnHitDelegate.Unbind();
            Projectile->Destroy();
        }
    }
    Super::EndPlay(EndPlayReason);
}
