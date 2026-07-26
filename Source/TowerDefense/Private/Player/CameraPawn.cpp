// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Player/CameraPawn.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"

ACameraPawn::ACameraPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(GetRootComponent());
}

void ACameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* const EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent); IsValid(EnhancedInput))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::AddMovement);
    }
}

void ACameraPawn::BeginPlay()
{
    Super::BeginPlay();
}

void ACameraPawn::AddMovement(const FInputActionValue& Input)
{
    checkf(Input.GetValueType() == EInputActionValueType::Axis3D, TEXT("Movement should be 3D axis movement"));
    const FVector MoveVector = Input.Get<FVector>();
    MoveRight(MoveVector.X);
    MoveForward(MoveVector.Y);
    MoveUp_World(MoveVector.Z);
}
