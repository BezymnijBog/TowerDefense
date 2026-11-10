// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "UnitEquipmentDefinition.generated.h"

class USkeletalMesh;
class UStaticMesh;

UENUM(BlueprintType)
enum class EUnitEquipmentSlot : uint8
{
    Body,
    Head,
    RightHand,
    LeftHand,
    Quiver
};

USTRUCT(BlueprintType)
struct TOWERDEFENSE_API FUnitEquipmentModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    FGameplayAttribute Attribute;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    float Bonus = 0.f;
};

// Shared description of an item's appearance and additive gameplay bonuses.
UCLASS(BlueprintType)
class TOWERDEFENSE_API UUnitEquipmentDefinition : public UDataAsset
{
    GENERATED_BODY()

public:
    bool Validate(EUnitEquipmentSlot ExpectedSlot, FText& OutError) const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    EUnitEquipmentSlot Slot = EUnitEquipmentSlot::Body;

    UPROPERTY(EditAnywhere,
              BlueprintReadOnly,
              Category = "Visuals",
              meta = (EditCondition = "Slot == EUnitEquipmentSlot::Body || Slot == EUnitEquipmentSlot::Head", EditConditionHides))
    TObjectPtr<USkeletalMesh> SkeletalMesh;

    UPROPERTY(EditAnywhere,
              BlueprintReadOnly,
              Category = "Visuals",
              meta = (EditCondition = "Slot != EUnitEquipmentSlot::Body && Slot != EUnitEquipmentSlot::Head", EditConditionHides))
    TObjectPtr<UStaticMesh> StaticMesh;

    // None uses the assembly editor's default socket for the slot.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
    FName SocketOrBone;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
    FTransform Offset = FTransform::Identity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (TitleProperty = "Attribute"))
    TArray<FUnitEquipmentModifier> Modifiers;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    FGameplayTagContainer GrantedTags;
};

USTRUCT(BlueprintType)
struct TOWERDEFENSE_API FUnitEquipmentLoadout
{
    GENERATED_BODY()

    bool Validate(FText& OutError) const;
    TArray<const UUnitEquipmentDefinition*> GetItems() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    TObjectPtr<UUnitEquipmentDefinition> Body;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    TObjectPtr<UUnitEquipmentDefinition> Head;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    TObjectPtr<UUnitEquipmentDefinition> RightHand;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    TObjectPtr<UUnitEquipmentDefinition> LeftHand;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    TObjectPtr<UUnitEquipmentDefinition> Quiver;
};
