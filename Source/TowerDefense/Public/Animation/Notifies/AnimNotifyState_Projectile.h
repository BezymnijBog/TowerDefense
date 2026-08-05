// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API UAnimNotifyState_Projectile : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,
                             UAnimSequenceBase* Animation,
                             float TotalDuration,
                             const FAnimNotifyEventReference& EventReference) override;

    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
    UPROPERTY(EditAnywhere, Category=NotifyState)
    FRotator ReleaseRotation;

    UPROPERTY(EditAnywhere, Category=NotifyState)
    FName SpawnSocketName;

    UPROPERTY(EditAnywhere, Category=NotifyState)
    FName TargetBlackboardKey;

    UPROPERTY()
    TWeakObjectPtr<AActor> ActiveProjectile;

    void LaunchProjectile(const USkeletalMeshComponent* MeshComp);
};
