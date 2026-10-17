// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TowerDefenseSettings.generated.h"

enum class EDayEventType : uint8;
class UDayChoiceDataAsset;

UCLASS(Config = Game, DefaultConfig)
class TOWERDEFENSE_API UTowerDefenseSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    static const UTowerDefenseSettings& Get();

    // The final day of each act is reserved for its boss.
    UPROPERTY(Config, EditDefaultsOnly, Category = "Run|Structure", meta = (ClampMin = "1", UIMin = "1"))
    int32 DaysPerAct = 7;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Run|Structure", meta = (ClampMin = "1", UIMin = "1"))
    int32 ActsPerRun = 3;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Run|Difficulty", meta = (ClampMin = "0.01", UIMin = "0.01"))
    float BaseEventDifficulty = 1.f;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Run|Difficulty", meta = (ClampMin = "0", UIMin = "0"))
    float DifficultyIncreasePerAct = 0.5f;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Run|Difficulty", meta = (ClampMin = "0", UIMin = "0"))
    float DifficultyIncreasePerDay = 0.1f;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Run|Rewards", meta = (ClampMin = "0", UIMin = "0"))
    TMap<EDayEventType, int32> MoneyReward;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Run")
    TSoftObjectPtr<UDayChoiceDataAsset> DayChoiceAsset;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Run")
    TSoftObjectPtr<UWorld> GameMap;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Menu|Transition", meta = (ClampMin = "0"))
    float GameFadeOutDuration = 0.6f;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Menu|Transition", meta = (ClampMin = "0"))
    float GameFadeInDuration = 0.8f;

    int32 GetReward(EDayEventType EventType) const;

    virtual FName GetCategoryName() const override;
    virtual FName GetSectionName() const override;
    virtual FText GetSectionDescription() const override;
};
