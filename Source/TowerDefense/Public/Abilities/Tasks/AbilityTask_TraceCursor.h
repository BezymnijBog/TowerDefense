// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TraceCursor.generated.h"

DECLARE_DELEGATE_OneParam(FCursorPositionChanged, const FHitResult&)

UCLASS()
class TOWERDEFENSE_API UAbilityTask_TraceCursor : public UAbilityTask
{
    GENERATED_BODY()

public:
    static UAbilityTask_TraceCursor* TraceCursorTask(UGameplayAbility* OwningAbility, FName InstanceName, APlayerController* TrackedController);

    explicit UAbilityTask_TraceCursor(const FObjectInitializer& ObjectInitializer);

    virtual void Activate() override;
    virtual void TickTask(float DeltaTime) override;

    FCursorPositionChanged CursorPositionChanged;

protected:
    UPROPERTY()
    TWeakObjectPtr<APlayerController> TrackingController;
};
