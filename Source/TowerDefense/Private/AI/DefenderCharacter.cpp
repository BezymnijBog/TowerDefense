// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/DefenderCharacter.h"

void ADefenderCharacter::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    SlotsOrientation = Transform.GetRotation();
}

TArray<FVector> ADefenderCharacter::GetSlotPoints() const
{
    return SlotPoints;
}

void ADefenderCharacter::BeginPlay()
{
    Super::BeginPlay();
    InitializeSlots();
}

void ADefenderCharacter::InitializeSlots()
{
    check(SlotPoints.IsEmpty());

    static constexpr int32 SlotsNum = 6;
    static constexpr double DeltaAngle = DOUBLE_TWO_PI / SlotsNum;

    SlotPoints.Reserve(SlotsNum);

    const FVector SlotsForward = SlotsOrientation.GetForwardVector();
    const FVector SlotsRight = SlotsOrientation.GetRightVector();

    for (int32 Idx = 0; Idx < SlotsNum; ++Idx)
    {
        double SinAngle;
        double CosAngle;
        FMath::SinCos(&SinAngle, &CosAngle, DeltaAngle * Idx);
        SlotPoints.Emplace(SlotsForward * CosAngle + SlotsRight * SinAngle);
    }
}
