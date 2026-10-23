// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/Projectile.h"

#include "Abilities/TowerDefenceTags.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/InterfaceFunctionLibrary.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
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
    if (!IsValid(Target) || !IsValid(InInstigator))
    {
        ReturnProjectile();
        return;
    }
    SetGenericTeamId(FGenericTeamId::GetTeamIdentifier(InInstigator));
    Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    bInFlight = true;
    PreviousLocation = GetActorLocation();
    SetActorHiddenInGame(false);
    SetActorTickEnabled(true);
    MovementComponent->Activate();
    MovementComponent->HomingTargetComponent = Target->GetRootComponent();
    MovementComponent->Velocity = MovementComponent->MaxSpeed * GetActorTransform().GetRotation().GetAxisX();
    ShotInstigator = InInstigator;
    GetWorld()->GetTimerManager().SetTimer(ActiveTimer, this, &ThisClass::ReturnProjectile, MaxFlyTime);
}

void AProjectile::Deactivate()
{
    bInFlight = false;
    SetActorTickEnabled(false);
    SetActorHiddenInGame(true);
    MovementComponent->Deactivate();
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
    AddTickPrerequisiteComponent(MovementComponent);
    Deactivate();
}

void AProjectile::ReturnProjectile()
{
    Deactivate();
    if (OnHitDelegate.IsBound())
    {
        OnHitDelegate.Execute(this);
    }
    else
    {
        Destroy();
    }
}

void AProjectile::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!bInFlight)
    {
        return;
    }
    if (!ShotInstigator.IsValid() || UInterfaceFunctionLibrary::IsActorDead(ShotInstigator.Get()))
    {
        ReturnProjectile();
        return;
    }
    FCollisionQueryParams Query(SCENE_QUERY_STAT(ProjectileFlight), false, this);
    Query.AddIgnoredActor(ShotInstigator.Get());
    FCollisionObjectQueryParams Objects;
    Objects.AddObjectTypesToQuery(ECC_Pawn);
    Objects.AddObjectTypesToQuery(ECC_WorldStatic);
    Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
    TArray<FHitResult> Hits;
    GetWorld()->SweepMultiByObjectType(Hits, PreviousLocation, GetActorLocation(), FQuat::Identity, Objects,
        FCollisionShape::MakeSphere(Collision->GetScaledSphereRadius()), Query);
    for (const FHitResult& Hit : Hits)
    {
        AActor* Other = Hit.GetActor();
        if (!IsValid(Other) || Other->IsA<AProjectile>() || UInterfaceFunctionLibrary::IsActorDead(Other))
        {
            continue;
        }
        const ETeamAttitude::Type Attitude = FGenericTeamId::GetAttitude(this, Other);
        if (Attitude == ETeamAttitude::Friendly)
        {
            continue;
        }
        if (Attitude == ETeamAttitude::Hostile)
        {
            bInFlight = false;
            AbilitySystem::SendGameplayEventToInstigator(ShotInstigator.Get(), Other, Action_Damage_Take);
            ReturnProjectile();
            return;
        }
        if (Hit.Component.IsValid() && Hit.Component->GetCollisionResponseToChannel(ECC_Visibility) == ECR_Block)
        {
            ReturnProjectile();
            return;
        }
    }
    PreviousLocation = GetActorLocation();
}
