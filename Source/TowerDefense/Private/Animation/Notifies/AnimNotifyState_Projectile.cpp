// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Animation/Notifies/AnimNotifyState_Projectile.h"

#include "Interfaces/InterfaceFunctionLibrary.h"

void UAnimNotifyState_Projectile::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                              UAnimSequenceBase* Animation,
                                              float TotalDuration,
                                              const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    UInterfaceFunctionLibrary::ActivateProjectile(MeshComp->GetOwner());
}

void UAnimNotifyState_Projectile::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    UInterfaceFunctionLibrary::LaunchActiveProjectile(MeshComp->GetOwner());
    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
