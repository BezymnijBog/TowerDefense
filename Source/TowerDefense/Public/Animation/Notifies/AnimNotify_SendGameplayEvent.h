// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_SendGameplayEvent.generated.h"

class UGameplayEffect;

UCLASS()
class TOWERDEFENSE_API UAnimNotify_SendGameplayEvent : public UAnimNotify
{
    GENERATED_BODY()

protected:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

    UPROPERTY(EditAnywhere, Category = AnimNotify)
    FName TargetBlackboardKey;

    UPROPERTY(EditAnywhere, Category = AnimNotify)
    FGameplayTag Tag;
};
