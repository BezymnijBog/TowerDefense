#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "Components/CameraPawnMovement.h"
#include "Components/SceneComponent.h"
#include "CQTest.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "Game/TowerDefenseGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/UnrealType.h"
#include "GameFramework/PlayerController.h"
#include "Player/CameraPawn.h"

TEST_CLASS_WITH_FLAGS(FCameraMovementTest, "TowerDefense.Camera", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
    FActorTestSpawner Spawner;
    ACameraPawn* Pawn = nullptr;
    UCameraPawnMovement* Movement = nullptr;

    BEFORE_EACH()
    {
        Pawn = &Spawner.SpawnActor<ACameraPawn>();
        Pawn->SetActorEnableCollision(false);
        Pawn->SetActorLocation(FVector(0., 0., 1000.));
        APlayerController& Controller = Spawner.SpawnActor<APlayerController>();
        Controller.SetAsLocalPlayerController();
        Controller.Possess(Pawn);
        Movement = CastChecked<UCameraPawnMovement>(Pawn->GetMovementComponent());
        Movement->SetUpdatedComponent(Pawn->GetRootComponent());
        ASSERT_THAT(IsTrue(Pawn->GetController() == &Controller));
        ASSERT_THAT(IsTrue(Controller.IsLocalController()));
        ASSERT_THAT(IsTrue(Movement->GetPawnOwner() == Pawn));
    }

    void Step()
    {
        Movement->TickComponent(1.f / 60.f, LEVELTICK_All, nullptr);
    }

    TEST_METHOD(KeyboardMovementCoastsAndStops)
    {
        Pawn->AddMovementInput(FVector::UpVector);
        Step();
        const float InitialSpeed = Movement->Velocity.Size();
        const FVector InitialLocation = Pawn->GetActorLocation();
        Step();
        ASSERT_THAT(IsTrue(InitialSpeed > 0.f));
        ASSERT_THAT(IsTrue(Movement->Velocity.Size() > 0.f && Movement->Velocity.Size() < InitialSpeed));
        ASSERT_THAT(IsTrue(Pawn->GetActorLocation().Z > InitialLocation.Z));
        for (int32 Frame = 0; Frame < 600; ++Frame)
        {
            Step();
        }
        ASSERT_THAT(IsTrue(Movement->Velocity.IsZero()));
        CheckHeightLimits();
    }

    void CheckHeightLimits()
    {
        const float MinHeight = Movement->GetMinHeight();
        const float MaxHeight = Movement->GetMaxHeight();
        for (const double Direction : { -1., 1. })
        {
            Movement->BeginDirectMovement();
            Movement->AddInputVector(FVector(10., 0., Direction * 2000.));
            Step();
            const double Limit = Direction < 0. ? MinHeight : MaxHeight;
            ASSERT_THAT(IsNear(Limit, Pawn->GetActorLocation().Z, 0.01));
            ASSERT_THAT(IsNear(0., Movement->Velocity.Z, 0.01));
            ASSERT_THAT(IsTrue(Movement->Velocity.X > 0.));
            Movement->EndDirectMovement();
            Movement->Velocity.Z = Direction * 600.;
            Step();
            ASSERT_THAT(IsNear(Limit, Pawn->GetActorLocation().Z, 0.01));
            ASSERT_THAT(IsNear(0., Movement->Velocity.Z, 0.01));
            Pawn->MoveUp_World(-Direction);
            Step();
            ASSERT_THAT(IsTrue((Pawn->GetActorLocation().Z - Limit) * Direction < 0.));
        }
    }

    TEST_METHOD(PitchLimitsAndMovementAxes)
    {
        CheckAdaptiveZoom();
        CheckPlayerStartPitch();
        Pawn->SetActorRotation(FRotator(0.f, 90.f, 0.f));
        Pawn->SetCameraPitch(90.f);
        ASSERT_THAT(IsNear(-80.f, static_cast<float>(Pawn->GetActorRotation().Pitch), 0.01f));
        Pawn->SetCameraPitch(0.f);
        ASSERT_THAT(IsNear(-30.f, static_cast<float>(Pawn->GetActorRotation().Pitch), 0.01f));
        for (const float Pitch : { 30.f, 60.f, 80.f })
        {
            Pawn->SetCameraPitch(Pitch);
            Pawn->MoveForward(1.f);
            ASSERT_THAT(IsTrue(Movement->ConsumeInputVector().Equals(FVector::RightVector, 0.001)));
            Pawn->MoveRight(1.f);
            ASSERT_THAT(IsTrue(Movement->ConsumeInputVector().Equals(-FVector::ForwardVector, 0.001)));
            Pawn->MoveUp_World(1.f);
            ASSERT_THAT(IsTrue(Movement->ConsumeInputVector().Equals(FVector::UpVector, 0.001)));
        }
    }

    void CheckAdaptiveZoom()
    {
        UInputAction* Action = NewObject<UInputAction>(Pawn);
        Action->ValueType = EInputActionValueType::Axis1D;
        FindFProperty<FObjectProperty>(ACameraPawn::StaticClass(), TEXT("ZoomAction"))->SetObjectPropertyValue_InContainer(Pawn, Action);
        UEnhancedInputComponent* Input = NewObject<UEnhancedInputComponent>(Pawn);
        Pawn->SetupPlayerInputComponent(Input);
        struct FZoomInput : FInputActionInstance
        {
            explicit FZoomInput(UInputAction* Action) : FInputActionInstance(Action)
            {
                Value = FInputActionValue(1.f);
                TriggerEvent = ETriggerEvent::Triggered;
            }
        };
        for (const double Height : { 500., 2000. })
        {
            Pawn->SetActorLocation(FVector(0., 0., Height));
            for (const auto& Binding : Input->GetActionEventBindings())
            {
                if (Binding->GetAction() == Action && Binding->GetTriggerEvent() == ETriggerEvent::Triggered)
                {
                    Binding->Execute(FZoomInput(Action));
                }
            }
            Step();
            ASSERT_THAT(IsNear(Height * 1.1, Pawn->GetActorLocation().Z, 0.01));
            for (const auto& Binding : Input->GetActionEventBindings())
            {
                if (Binding->GetAction() == Action && Binding->GetTriggerEvent() == ETriggerEvent::Completed)
                {
                    Binding->Execute(FInputActionInstance(Action));
                }
            }
            Step();
            ASSERT_THAT(IsTrue(Pawn->GetActorLocation().Z > Height * 1.1));
        }
        Movement->StopMovementImmediately();
    }

    void CheckPlayerStartPitch()
    {
        ATowerDefenseGameMode& GameMode = Spawner.SpawnActor<ATowerDefenseGameMode>();
        GameMode.DefaultPawnClass = ACameraPawn::StaticClass();
        APlayerStart& Start = Spawner.SpawnActor<APlayerStart>();
        Start.GetRootComponent()->SetMobility(EComponentMobility::Movable);
        Start.SetActorLocation(FVector(1000., 2000., 3000.));
        Start.SetActorRotation(FRotator(-60.f, 45.f, 0.f));
        ASSERT_THAT(IsNear(-60.f, static_cast<float>(Start.GetActorRotation().Pitch), 0.01f));
        APlayerController& Controller = Spawner.SpawnActor<APlayerController>();
        Controller.SetAsLocalPlayerController();
        GameMode.RestartPlayerAtPlayerStart(&Controller, &Start);
        APawn* SpawnedPawn = Controller.GetPawn();
        ASSERT_THAT(IsNotNull(SpawnedPawn));
        ASSERT_THAT(IsTrue(SpawnedPawn->GetActorRotation().Equals(Start.GetActorRotation(), 0.01)));
        UCameraComponent* Camera = SpawnedPawn->FindComponentByClass<UCameraComponent>();
        ASSERT_THAT(IsNotNull(Camera));
        ASSERT_THAT(IsTrue(Camera->GetComponentRotation().Equals(Start.GetActorRotation(), 0.01)));
    }

    TEST_METHOD(DirectMovementCoastsButHoldingStillStopsIt)
    {
        Movement->BeginDirectMovement();
        const FVector InitialLocation = Pawn->GetActorLocation();
        Movement->AddInputVector(FVector(10., 0., 0.));
        Step();
        ASSERT_THAT(IsTrue(Pawn->GetActorLocation().Equals(InitialLocation + FVector(10., 0., 0.), 0.01)));
        Movement->EndDirectMovement();
        Step();
        ASSERT_THAT(IsTrue(Movement->Velocity.X > 0.f && Movement->Velocity.X < 600.f));
        Movement->BeginDirectMovement();
        Movement->AddInputVector(FVector(10., 0., 0.));
        Step();
        Step();
        Movement->EndDirectMovement();
        Step();
        ASSERT_THAT(IsTrue(Movement->Velocity.IsZero()));
    }

    TEST_METHOD(CancelDiscardsPendingMovement)
    {
        const FVector InitialLocation = Pawn->GetActorLocation();
        Movement->BeginDirectMovement();
        Movement->AddInputVector(FVector(10., 0., 0.));
        Movement->StopMovementImmediately();
        Step();
        ASSERT_THAT(IsTrue(Pawn->GetActorLocation().Equals(InitialLocation)));
        ASSERT_THAT(IsTrue(Movement->Velocity.IsZero()));
    }

    TEST_METHOD(ReleasePreservesAccumulatedInputBeforeMovementTick)
    {
        const FVector InitialLocation = Pawn->GetActorLocation();
        Movement->BeginDirectMovement();
        Movement->AddInputVector(FVector(4., 0., 0.));
        Movement->AddInputVector(FVector(6., 0., 0.));
        Movement->EndDirectMovement();
        Step();
        ASSERT_THAT(IsTrue(Pawn->GetActorLocation().Equals(InitialLocation + FVector(10., 0., 0.), 0.01)));
        ASSERT_THAT(IsTrue(Movement->GetPendingInputVector().IsZero()));
        Step();
        ASSERT_THAT(IsTrue(Movement->Velocity.X > 0.f && Movement->Velocity.X < 600.f));
        Movement->StopMovementImmediately();
        Movement->AddInputVector(FVector::UpVector);
        Step();
        ASSERT_THAT(IsTrue(Movement->Velocity.Z > 0.f));
    }

    TEST_METHOD(MovementEventWithoutHoldStartedStopsCoasting)
    {
        UInputAction* PanAction = NewObject<UInputAction>(Pawn);
        PanAction->ValueType = EInputActionValueType::Axis2D;
        FObjectProperty* Property = FindFProperty<FObjectProperty>(ACameraPawn::StaticClass(), TEXT("PanMove"));
        ASSERT_THAT(IsNotNull(Property));
        Property->SetObjectPropertyValue_InContainer(Pawn, PanAction);
        UEnhancedInputComponent* Input = NewObject<UEnhancedInputComponent>(Pawn);
        Pawn->SetupPlayerInputComponent(Input);
        Movement->Velocity = FVector(600., 0., 0.);
        bool bDispatched = false;
        for (const auto& Binding : Input->GetActionEventBindings())
        {
            if (Binding->GetAction() == PanAction && Binding->GetTriggerEvent() == ETriggerEvent::Triggered)
            {
                // Reproduce chord suppression: PanMove arrives without PanActivation.Started.
                Binding->Execute(FInputActionInstance(PanAction));
                bDispatched = true;
            }
        }
        ASSERT_THAT(IsTrue(bDispatched));
        // No viewport in this fixture: acquiring a pan plane fails, but old inertia must stop.
        ASSERT_THAT(IsTrue(Movement->Velocity.IsZero()));
    }

    TEST_METHOD(ReversingDirectMovementAlwaysReplacesInertia)
    {
        for (int32 Gesture = 0; Gesture < 6; ++Gesture)
        {
            const double Direction = Gesture % 2 == 0 ? 1. : -1.;
            Movement->BeginDirectMovement();
            ASSERT_THAT(IsTrue(Movement->Velocity.IsZero()));
            Movement->AddInputVector(FVector(10. * Direction, 0., 0.));
            Step();
            ASSERT_THAT(IsTrue(Movement->Velocity.X * Direction > 0.));
            Movement->EndDirectMovement();
            Step();
            ASSERT_THAT(IsTrue(Movement->Velocity.X * Direction > 0.));
        }
    }
};

#endif
