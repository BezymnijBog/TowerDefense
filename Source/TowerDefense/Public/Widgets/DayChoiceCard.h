// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/RunSubsystem.h"
#include "DayChoiceCard.generated.h"

class UImage;
class UButton;
class UTextBlock;

UCLASS()
class TOWERDEFENSE_API UDayChoiceCard : public UUserWidget
{
    GENERATED_BODY()

public:
    void Generate(const FDayChoice& DayChoice);
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SelectButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> EventIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TitleText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DescriptionText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> RewardText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DifficultyText;

private:
    UFUNCTION()
    void SelectChoice();

    UPROPERTY()
    FDayChoice Choice;
};
