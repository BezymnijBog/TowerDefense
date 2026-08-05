// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/Projectile.h"

#include "Abilities/TowerDefenceTags.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
    DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
    SetRootComponent(DefaultRoot);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    StaticMesh->SetupAttachment(DefaultRoot);
    StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->SetupAttachment(DefaultRoot);
    Collision->InitSphereRadius(10.f);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
    MovementComponent->InitialSpeed = 5000.f;
    MovementComponent->MaxSpeed = 5000.f;
    MovementComponent->Velocity = FVector::ZeroVector;
}

void AProjectile::Activate(const AActor* Target, AActor* InInstigator)
{
    SetGenericTeamId(FGenericTeamId::GetTeamIdentifier(InInstigator));
    Collision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
    Collision->IgnoreActorWhenMoving(InInstigator, true);
    MovementComponent->HomingTargetComponent = Target->GetRootComponent();
    MovementComponent->Velocity = MovementComponent->MaxSpeed * GetActorTransform().GetRotation().GetAxisX();
    ShotInstigator = InInstigator;
    GetWorld()->GetTimerManager().SetTimer(ActiveTimer, this, &ThisClass::Deactivate, MaxFlyTime);
}

void AProjectile::Deactivate()
{
    GetWorld()->GetTimerManager().ClearTimer(ActiveTimer);
    SetActorLocation(FVector::ZeroVector);
    Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MovementComponent->HomingTargetComponent = nullptr;
    MovementComponent->Velocity = FVector::ZeroVector;
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
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnProjectileHit);
}

void AProjectile::OnProjectileHit(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int, bool, const FHitResult&)
{
    if (!IsValid(OtherActor) || GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile)
    {
        return;
    }

    if (const TStrongObjectPtr<AActor> InstigatorStrong = ShotInstigator.Pin(); InstigatorStrong.IsValid())
    {
        AbilitySystem::SendGameplayEventToInstigator(InstigatorStrong.Get(), OtherActor, Action_Damage_Take);
        std::ignore = OnHitDelegate.ExecuteIfBound(this);
    }
}
