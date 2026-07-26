// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemyCharacter;
class USplineComponent;

UCLASS()
class TOWERDEFENSE_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "EnemySpawner")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "EnemySpawner")
    TSubclassOf<AEnemyCharacter> EnemiesToSpawn;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "EnemySpawner")
    TObjectPtr<USplineComponent> EnemyPath;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemySpawner")
    float SpawnInterval = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemySpawner")
    float SpawnDelay = 2.f;

private:
    void SpawnEnemy();

    FVector SpawnLocation = FVector::ZeroVector;
    FTimerHandle SpawnTimer;
};
