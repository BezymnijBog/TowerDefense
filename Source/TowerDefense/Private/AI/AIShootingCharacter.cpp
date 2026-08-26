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
    ActiveProjectile = ReserveProjectiles.IsEmpty() ? GetWorld()->SpawnActor<AProjectile>(ProjectileClass) : ReserveProjectiles.Pop();
    ProjectileMesh->SetVisibility(true);
}

void AAIShootingCharacter::LaunchActiveProjectile()
{
    AAIController* const CurrentController = GetInstigatorController<AAIController>();
    if (!IsValid(CurrentController) || !ActiveProjectile.IsValid())
    {
        return;
    }

    const AActor* const Target = Cast<AActor>(CurrentController->GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKey));
    if (!IsValid(Target))
    {
        return;
    }

    ProjectileMesh->SetVisibility(false);

    ActiveProjectile->SetActorLocation(GetMesh()->GetSocketLocation(SpawnSocketName));
    ActiveProjectile->SetActorRotation(Owner->GetActorQuat() * ReleaseRotation.Quaternion());

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
