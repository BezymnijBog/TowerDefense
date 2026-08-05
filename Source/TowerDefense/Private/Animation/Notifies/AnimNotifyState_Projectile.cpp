// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Animation/Notifies/AnimNotifyState_Projectile.h"

#include "Actors/Projectile.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/InterfaceFunctionLibrary.h"

namespace
{
void SetProjectileComponentVisibility(const AActor* Owner, bool bIsVisible)
{
    if (UStaticMeshComponent* const Component = Owner->GetComponentByClass<UStaticMeshComponent>(); IsValid(Component))
    {
        Component->SetVisibility(bIsVisible);
    }
}
} // namespace

void UAnimNotifyState_Projectile::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                              UAnimSequenceBase* Animation,
                                              float TotalDuration,
                                              const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    ActiveProjectile = UInterfaceFunctionLibrary::ActivateProjectile(MeshComp->GetOwner());
    SetProjectileComponentVisibility(MeshComp->GetOwner(), true);
}

void UAnimNotifyState_Projectile::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    LaunchProjectile(MeshComp);
    Super::NotifyEnd(MeshComp, Animation, EventReference);
}

void UAnimNotifyState_Projectile::LaunchProjectile(const USkeletalMeshComponent* MeshComp)
{
    AActor* const Owner = MeshComp->GetOwner();
    AAIController* const Controller = Owner->GetInstigatorController<AAIController>();
    if (!IsValid(Controller) || !ActiveProjectile.IsValid())
    {
        return;
    }

    const AActor* const Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKey));
    if (!IsValid(Target))
    {
        return;
    }

    SetProjectileComponentVisibility(MeshComp->GetOwner(), false);
    ActiveProjectile->SetActorLocation(MeshComp->GetSocketLocation(SpawnSocketName));
    ActiveProjectile->SetActorRotation(Owner->GetActorQuat() * ReleaseRotation.Quaternion());
    UInterfaceFunctionLibrary::LaunchActiveProjectile(Owner, Target);
    ActiveProjectile = nullptr;
}
