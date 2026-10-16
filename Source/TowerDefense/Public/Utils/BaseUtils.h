// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/GameplayStatics.h"
#include "BaseUtils.generated.h"

class ATowerDefenseAICharacter;
class UAbilityTask;
class UGameplayAbility;

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
struct FHireWidgetRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    uint32 Cost = 0;

    UPROPERTY(EditAnywhere)
    TSubclassOf<ATowerDefenseAICharacter> Class;

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

namespace Utils
{
template <std::derived_from<AActor> T>
T* GetActorOfClass(UObject* WorldContext)
{
    return Cast<T>(UGameplayStatics::GetActorOfClass(WorldContext, T::StaticClass()));
}

template <std::derived_from<UWorldSubsystem> TSubsystem>
TSubsystem* GetWorldSubsystem(const UObject* WorldContext)
{
    const UWorld* World = IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr;
    return IsValid(World) ? World->GetSubsystem<TSubsystem>() : nullptr;
}

template <std::derived_from<UGameInstanceSubsystem> TSubsystem>
TSubsystem* GetGameInstanceSubsystem(const UWidget* GameInstanceContext)
{
    const UGameInstance* GameInstance = IsValid(GameInstanceContext) ? GameInstanceContext->GetGameInstance() : nullptr;
    return IsValid(GameInstance) ? GameInstance->GetSubsystem<TSubsystem>() : nullptr;
}

template <std::derived_from<UGameInstanceSubsystem> TSubsystem>
TSubsystem* GetGameInstanceSubsystem(const AActor* GameInstanceContext)
{
    const UGameInstance* GameInstance = IsValid(GameInstanceContext) ? GameInstanceContext->GetGameInstance() : nullptr;
    return IsValid(GameInstance) ? GameInstance->GetSubsystem<TSubsystem>() : nullptr;
}
} // namespace Utils
