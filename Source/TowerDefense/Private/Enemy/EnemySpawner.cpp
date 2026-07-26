// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Enemy/EnemySpawner.h"

#include "Components/SplineComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyController.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    EnemyPath = CreateDefaultSubobject<USplineComponent>(TEXT("SplinePath"));
    EnemyPath->SetupAttachment(Mesh);
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::SpawnEnemy, SpawnInterval, true, SpawnDelay);
    SpawnLocation = EnemyPath->GetWorldLocationAtSplinePoint(0);
}

void AEnemySpawner::SpawnEnemy()
{
    UWorld* const World = GetWorld();
    if (!IsValid(World))
    {
        return;
    }

    AEnemyCharacter* const Spawned = World->SpawnActor<AEnemyCharacter>(EnemiesToSpawn, SpawnLocation, GetActorRotation());
    if (!IsValid(Spawned))
    {
        return;
    }

    if (!IsValid(Spawned->Controller))
    {
        Spawned->SpawnDefaultController();
    }

    if (AEnemyController* const Controller = Spawned->GetController<AEnemyController>(); IsValid(Controller))
    {
        Controller->SetPathSpline(EnemyPath);
    }
}
