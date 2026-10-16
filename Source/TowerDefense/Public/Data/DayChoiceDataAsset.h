// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DayChoiceDataAsset.generated.h"

enum class EDayEventType : uint8;

USTRUCT()
struct FDayChoiceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere)
    FText Description;
};

UCLASS()
class TOWERDEFENSE_API UDayChoiceDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "DayChoice")
    TMap<EDayEventType, FDayChoiceData> EventData;
};
