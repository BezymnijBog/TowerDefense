// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/Projectile.h"

#include "Abilities/TowerDefenceTags.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AProjectile::AProjectile()
{
    SphereRoot = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SetRootComponent(SphereRoot);
    SphereRoot->InitSphereRadius(10.f);

    VisualEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
    VisualEffect->SetupAttachment(SphereRoot);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
    MovementComponent->bIsHomingProjectile = true;
    MovementComponent->InitialSpeed = 500.f;
    MovementComponent->MaxSpeed = 500.f;

    TeamId.SetAttitudeSolver(Teams::AttitudeSolveFunction);

    PrimaryActorTick.bCanEverTick = true;
}

void AProjectile::Activate(const FTransform& Transform, const AActor* Target, AActor* InInstigator)
{
    SetActorTransform(Transform);
    SphereRoot->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
    SphereRoot->IgnoreActorWhenMoving(InInstigator, true);
    MovementComponent->HomingTargetComponent = Target->GetRootComponent();
    MovementComponent->Velocity = MovementComponent->InitialSpeed * Transform.GetRotation().GetAxisX();
    VisualEffect->Activate();
    ShotInstigator = InInstigator;
}

void AProjectile::Deactivate()
{
    SetActorLocation(FVector::ZeroVector);
    SphereRoot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MovementComponent->HomingTargetComponent = nullptr;
    MovementComponent->Velocity = FVector::ZeroVector;
    VisualEffect->Deactivate();
    ShotInstigator = nullptr;
}

FGenericTeamId AProjectile::GetGenericTeamId() const
{
    return TeamId;
}

void AProjectile::SetGenericTeamId(const FGenericTeamId& TeamID)
{
    TeamId = TeamID;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    SphereRoot->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnProjectileHit);
}

void AProjectile::OnProjectileHit(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int, bool, const FHitResult&)
{
    if (!IsValid(OtherActor) || GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile)
    {
        return;
    }

    if (const TStrongObjectPtr<AActor> InstigatorStrong = ShotInstigator.Pin(); InstigatorStrong.IsValid())
    {
        AbilitySystem::SendGameplayEventToTarget(InstigatorStrong.Get(), OtherActor, Action_Damage_Take);
        std::ignore = OnHitDelegate.ExecuteIfBound(this);
    }
}
