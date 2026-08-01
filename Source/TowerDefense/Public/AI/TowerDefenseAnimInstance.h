// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TowerDefenseAnimInstance.generated.h"

class ATowerDefenseAICharacter;

UCLASS()
class TOWERDEFENSE_API UTowerDefenseAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeBeginPlay() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
    float GroundSpeed = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
    bool bIsDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
    bool bDeathAnimation = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
    TWeakObjectPtr<ATowerDefenseAICharacter> OwningCharacter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
    TWeakObjectPtr<UPawnMovementComponent> OwnerMovement;
};
