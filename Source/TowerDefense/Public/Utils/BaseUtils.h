// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "BaseUtils.generated.h"

class ATowerDefenseAICharacter;
class UAbilityTask;

namespace Teams
{
UENUM(BlueprintType)
enum ETeamId : uint8
{
    TeamPlayer = 0,
    TeamEnemy = 1,
    TeamNone = 255
};

ETeamAttitude::Type AttitudeSolveFunction(FGenericTeamId Left, FGenericTeamId Right);
} // namespace Teams

namespace Collision
{
const FName NoCollisionProfile = TEXT("NoCollision");
}

USTRUCT()
struct FHireInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    uint32 Cost = 0;

    UPROPERTY(EditAnywhere)
    TSubclassOf<ATowerDefenseAICharacter> Class;
};

USTRUCT()
struct FHireWidgetRow : public FTableRowBase, public FHireInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FText Name;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> Image;

    UPROPERTY(EditAnywhere)
    FVector2f ImageSize;
};

namespace AbilitySystem
{
void SendGameplayEventToInstigator(AActor* Instigator, AActor* Target, FGameplayTag Tag, float Magnitude = 0.f);
void SendGameplayEventToTarget(AActor* Instigator, AActor* Target, FGameplayTag Tag, float Magnitude = 0.f);
void EndTask(UAbilityTask* Task);
} // namespace AbilitySystem

// namespace Teams
