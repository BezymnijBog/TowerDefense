// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartWaveWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class TOWERDEFENSE_API UStartWaveWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category = WaveStart, meta = (BindWidget))
    TObjectPtr<UImage> HotKeyImage;

    UPROPERTY(EditDefaultsOnly, Category = WaveStart, meta = (BindWidget))
    TObjectPtr<UTextBlock> Text;
};
