// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressBarWidget.generated.h"

class UProgressBar;

UCLASS()
class TOWERDEFENSE_API UProgressBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetPercent(float Percent);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UProgressBar> ProgressBar;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ProgressPercent;
};
