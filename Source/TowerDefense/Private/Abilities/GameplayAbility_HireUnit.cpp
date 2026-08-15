// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/GameplayAbility_HireUnit.h"

#include "Abilities/Tasks/AbilityTask_TraceCursor.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/UnitPreview.h"
#include "AI/TowerDefenseAICharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Subsystems/WorldRoadSubsystem.h"
#include "Utils/BaseUtils.h"

namespace
{
FVector SnappedToGrid(const FVector& Input, const FVector& GridSize, const FVector& GridOffset)
{
    const auto SnappedToAxis = [](double Value, double Size, double Offset)
    {
        return FMath::RoundHalfFromZero((Value - Offset) / Size) * Size + Offset;
    };

    return { SnappedToAxis(Input.X, GridSize.X, GridOffset.X),
             SnappedToAxis(Input.Y, GridSize.Y, GridOffset.Y),
             SnappedToAxis(Input.Z, GridSize.Z, GridOffset.Z) };
}
} // namespace

void UGameplayAbility_HireUnit::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    check(TriggerEventData);
    APlayerController* const PlayerController = Cast<APlayerController>(ActorInfo->AvatarActor.Get());
    ActivateInputContext(PlayerController);
    UpdateHireUnit(*TriggerEventData);

    CursorTask = UAbilityTask_TraceCursor::TraceCursorTask(this, TEXT("CursorTraceTask"), PlayerController);
    CursorTask->CursorPositionChanged.BindUObject(this, &ThisClass::OnPositionChanged);
    CursorTask->ReadyForActivation();

    WaitUpdateTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TriggerEventData->EventTag);
    WaitUpdateTask->EventReceived.AddUniqueDynamic(this, &ThisClass::UpdateHireUnit);
    WaitUpdateTask->ReadyForActivation();
}

void UGameplayAbility_HireUnit::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateEndAbility,
                                           bool bWasCancelled)
{
    AbilitySystem::EndTask(CursorTask);
    AbilitySystem::EndTask(WaitUpdateTask);
    DeactivateInputContext(Cast<APlayerController>(ActorInfo->AvatarActor));
    if (IsValid(PreviewActor))
    {
        PreviewActor->Destroy();
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_HireUnit::UpdateHireUnit(FGameplayEventData Payload)
{
    if (!IsValid(PreviewActor))
    {
        PreviewActor = GetWorld()->SpawnActor<AUnitPreview>(PreviewActorClass);
    }
    HireClass = TSubclassOf<ATowerDefenseAICharacter>(const_cast<UClass*>(Cast<UClass>(Payload.OptionalObject)));
    check(IsValid(HireClass));
    const USkeletalMeshComponent* const SpawnMesh = HireClass.GetDefaultObject()->GetMesh();
    USkeletalMeshComponent* const PreviewMesh = PreviewActor->GetMesh();
    PreviewMesh->SetSkeletalMesh(SpawnMesh->GetSkeletalMeshAsset());
    PreviewMesh->SetRelativeRotation(SpawnMesh->GetRelativeRotation());
    MeshRelativeLocation = SpawnMesh->GetRelativeLocation();
    HireCost = -FMath::Abs(Payload.EventMagnitude);
}

void UGameplayAbility_HireUnit::OnHireConfirmed()
{
    if (!bCanSpawn || !CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) || !CurrentActorInfo->AbilitySystemComponent.IsValid())
    {
        return;
    }

    UAbilitySystemComponent* const AbilitySystemComponent = CurrentActorInfo->AbilitySystemComponent.Get();
    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(HireCostEffect);
    SpecHandle = UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Action_Money_Change, HireCost);
    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

    const FRotator SpawnRotation = PreviewActor->GetActorRotation();
    GetWorld()->SpawnActor<ATowerDefenseAICharacter>(HireClass, PreviewActor->GetActorLocation(), SpawnRotation);
    UpdatePreviewMaterial();
}

void UGameplayAbility_HireUnit::OnHireCancelled()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_HireUnit::RotatePreviewMesh(const FInputActionValue& RotationValue)
{
    if (RotationValue.GetValueType() != EInputActionValueType::Axis1D)
    {
        return;
    }

    PreviewActor->SetActorRotation(PreviewActor->GetActorRotation() + FRotator(0., RotationValue.Get<float>() * 15., 0.));
}

void UGameplayAbility_HireUnit::OnPositionChanged(const FHitResult& HitResult)
{
    PreviewActor->SetActorLocation(SnappedToGrid(HitResult.ImpactPoint - MeshRelativeLocation, GridSize, GridOffset), true);
    UpdatePreviewMaterial();
}

void UGameplayAbility_HireUnit::ActivateInputContext(const APlayerController* Controller)
{
    if (!IsValid(Controller))
    {
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* const Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer());
        IsValid(Subsystem))
    {
        Subsystem->AddMappingContext(HireInputContext, 1);
    }

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(Controller->InputComponent); IsValid(EnhancedInput))
    {
        EnhancedInput->BindAction(ConfirmHire, ETriggerEvent::Triggered, this, &ThisClass::OnHireConfirmed);
        EnhancedInput->BindAction(CancelHire, ETriggerEvent::Triggered, this, &ThisClass::OnHireCancelled);
        EnhancedInput->BindAction(RotatePreview, ETriggerEvent::Triggered, this, &ThisClass::RotatePreviewMesh);
    }
}

void UGameplayAbility_HireUnit::DeactivateInputContext(const APlayerController* Controller) const
{
    if (!IsValid(Controller))
    {
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* const Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer());
        IsValid(Subsystem))
    {
        Subsystem->RemoveMappingContext(HireInputContext);
    }
}

void UGameplayAbility_HireUnit::UpdatePreviewMaterial()
{
    bCanSpawn = HasEnoughResources();
    if (const UWorldRoadSubsystem* const Subsystem = GetWorld()->GetSubsystem<UWorldRoadSubsystem>(); IsValid(Subsystem))
    {
        bCanSpawn &= Subsystem->CanSpawnUnit(PreviewActor->GetActorLocation());
    }
    bCanSpawn &= !PreviewActor->HasCollisionIntersections();
    PreviewActor->GetMesh()->SetMaterial(0, bCanSpawn ? AllowSpawnMaterial : ForbidSpawnMaterial);
}

bool UGameplayAbility_HireUnit::HasEnoughResources() const
{
    bool bResult = true;
    for (const FGameplayModifierInfo& Modifier : HireCostEffect.GetDefaultObject()->Modifiers)
    {
        bool bAttributeFound = true;
        const float Value = CurrentActorInfo->AbilitySystemComponent->GetGameplayAttributeValue(Modifier.Attribute, bAttributeFound);
        bResult &= bAttributeFound && Value >= FMath::Abs(HireCost);
    }
    return bResult;
}
