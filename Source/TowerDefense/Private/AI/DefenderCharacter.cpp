// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/DefenderCharacter.h"

#include "AI/AttackSlot.h"
#include "Components/AttackSlotComponent.h"
#include "Components/CapsuleComponent.h"

ADefenderCharacter::ADefenderCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    SlotsComponent = CreateDefaultSubobject<UAttackSlotComponent>(TEXT("AttackSlots"));
}

void ADefenderCharacter::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    SlotsComponent->SetBaseTransform(Transform);
}

TArray<FAttackSlot> ADefenderCharacter::GetSlotPoints() const
{
    return SlotsComponent->GetSlots();
}

void ADefenderCharacter::BeginPlay()
{
    Super::BeginPlay();
    InitializeSlots();
}

void ADefenderCharacter::InitializeSlots()
{
    static constexpr int32 SlotsNum = 8;
    static constexpr double DeltaAngle = DOUBLE_TWO_PI / SlotsNum;

    const float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
    const float Radius = FMath::CeilToFloat(CapsuleRadius * FMath::Sqrt(2.f / (1.f - FMath::Cos(DeltaAngle))));
    TArray<FVector> SlotPoints;
    SlotPoints.Reserve(SlotsNum);

    for (int32 Idx = 0; Idx < SlotsNum; ++Idx)
    {
        double SinAngle;
        double CosAngle;
        FMath::SinCos(&SinAngle, &CosAngle, DeltaAngle * Idx);
        SlotPoints.Emplace(Radius * (FVector::ForwardVector * CosAngle + FVector::RightVector * SinAngle));
    }
    SlotsComponent->InitializeSlotsLocal(SlotPoints);
}
