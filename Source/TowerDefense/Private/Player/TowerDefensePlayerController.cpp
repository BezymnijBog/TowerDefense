// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/TowerDefensePlayerController.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemComponent.h"
#include "Actors/PlayerHUD.h"
#include "Actors/DefenderBase.h"
#include "Attributes/PlayerAttributeSet.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TowerDefense.h"
#include "Widgets/PlayerHudWidget.h"

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

ATowerDefensePlayerController::ATowerDefensePlayerController()
{
    bIsTouch = false;
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void ATowerDefensePlayerController::SetBuildingMode(FBuildingInfo InBuildingInfo)
{
    BuildingInfo = MoveTemp(InBuildingInfo);
    bIsBuildingMode = IsValid(BuildingInfo.BuildingClass);
    if (bIsBuildingMode)
    {
        FHitResult HitResult;
        GetHitResultUnderCursor(ECC_Visibility, true, HitResult);
        const FTransform SpawnTransform(FRotator::ZeroRotator, HitResult.Location);
        PreviewActor = GetWorld()->SpawnActor<ADefenderBase>(BuildingInfo.BuildingClass, SpawnTransform);
        PreviewActor->UpdateIsBuildingAllowed();
    }
    else if (IsValid(PreviewActor))
    {
        PreviewActor->Destroy();
    }
}

UAbilitySystemComponent* ATowerDefensePlayerController::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ATowerDefensePlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateBuildingPreview();
}

void ATowerDefensePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (!IsValid(BuildAllowedMaterial) || !IsValid(BuildRejectedMaterial))
    {
        UE_LOG(LogTowerDefense, Error, TEXT("Invalid materials for building"));
    }
    InitializeAbilitySystem();
}

void ATowerDefensePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        //EnhancedInputComponent->BindAction(SetLocationClickAction, ETriggerEvent::Started, this, &ThisClass::OnInputStarted);
        EnhancedInputComponent->BindAction(SetLocationClickAction, ETriggerEvent::Triggered, this, &ThisClass::OnSetDestinationTriggered);
        //EnhancedInputComponent->BindAction(SetLocationClickAction, ETriggerEvent::Completed, this, &ThisClass::OnSetDestinationReleased);
        //EnhancedInputComponent->BindAction(SetLocationClickAction, ETriggerEvent::Canceled, this, &ThisClass::OnSetDestinationReleased);

        EnhancedInputComponent->BindAction(SetLocationTouchAction, ETriggerEvent::Started, this, &ThisClass::OnInputStarted);
        EnhancedInputComponent->BindAction(SetLocationTouchAction, ETriggerEvent::Triggered, this, &ThisClass::OnTouchTriggered);
        EnhancedInputComponent->BindAction(SetLocationTouchAction, ETriggerEvent::Completed, this, &ThisClass::OnTouchReleased);
        EnhancedInputComponent->BindAction(SetLocationTouchAction, ETriggerEvent::Canceled, this, &ThisClass::OnTouchReleased);
    }
    else
    {
        UE_LOG(
            LogTowerDefense,
            Error,
            TEXT(
                "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."),
            *GetNameSafe(this));
    }
}

void ATowerDefensePlayerController::OnInputStarted()
{
    if (!bIsBuildingMode)
    {
        StopMovement();
    }
}

void ATowerDefensePlayerController::OnSetDestinationTriggered()
{
    if (!IsValid(PreviewActor) || !PreviewActor->IsAllowedToBuild())
    {
        return;
    }
    AbilitySystem::SendGameplayEventToInstigator(this, this, Action_Money_Spend);
    PreviewActor->FinishBuilding();
    PreviewActor = nullptr;
    SetBuildingMode({});
}

void ATowerDefensePlayerController::OnSetDestinationReleased()
{
    if (FollowTime <= ShortPressThreshold)
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector::OneVector, true, true, ENCPoolMethod::None, true);
    }

    FollowTime = 0.f;
}

void ATowerDefensePlayerController::OnTouchTriggered()
{
    bIsTouch = true;
    OnSetDestinationTriggered();
}

void ATowerDefensePlayerController::OnTouchReleased()
{
    bIsTouch = false;
    OnSetDestinationReleased();
}

void ATowerDefensePlayerController::InitializeAbilitySystem()
{
    if (!IsValid(AbilitySystemComponent))
    {
        return;
    }

    PlayerAttributes = AbilitySystemComponent->AddSet<UPlayerAttributeSet>();
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMoneyAttribute()).AddUObject(this, &ThisClass::OnMoneyChanged);

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilityClasses)
    {
        const FGameplayAbilitySpecHandle& AbilitySpec = GivenAbilities.Emplace_GetRef(AbilitySystemComponent->GiveAbility(AbilityClass));
        if (!AbilitySystemComponent->TryActivateAbility(AbilitySpec))
        {
            UE_LOG(LogTowerDefense, Warning, TEXT("Failed to activate ability %s for %s"), *AbilitySpec.ToString(), *GetName());
        }
    }
}

void ATowerDefensePlayerController::OnMoneyChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
    if (const APlayerHUD* const HUD = GetHUD<APlayerHUD>(); HUD && HUD->GetHUDWidget())
    {
        HUD->GetHUDWidget()->SetMoney(OnAttributeChangeData.NewValue);
    }
}

void ATowerDefensePlayerController::UpdateBuildingPreview() const
{
    if (!bIsBuildingMode)
    {
        return;
    }

    static constexpr double TraceLength = 100000.;
    static const FVector GridSize = FVector::OneVector * 100.;
    static const FVector GridOffset = FVector::ForwardVector * 50. + FVector::RightVector * 50. + FVector::UpVector * 0.;

    FVector TraceStart;
    FVector TraceDir;
    DeprojectMousePositionToWorld(TraceStart, TraceDir);
    FCollisionQueryParams Query(TEXT("Building trace"), false, GetPawn());
    Query.AddIgnoredActor(PreviewActor);

    if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceStart + TraceDir * TraceLength, ECC_Visibility, Query))
    {
        PreviewActor->SetActorLocation(SnappedToGrid(HitResult.Location, GridSize, GridOffset));
    }
}
