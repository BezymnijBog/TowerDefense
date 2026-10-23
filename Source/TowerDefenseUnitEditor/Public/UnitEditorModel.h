// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Interfaces/AttackerInterface.h"
#include "Utils/AbilityDescription.h"
#include "UnitEditorModel.generated.h"

class UBlueprint;
class UDataTable;
class USkeletalMesh;
class UAnimInstance;
class AAIController;
class ATowerDefenseAICharacter;
class AProjectile;
class UAbilitySystemComponent;

UENUM()
enum class EUnitEditorTeam : uint8
{
    Defender,
    Enemy
};

USTRUCT()
struct TOWERDEFENSEUNITEDITOR_API FUnitEditorAttribute
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = Attribute)
    FName Name;

    UPROPERTY(EditAnywhere, Category = Attribute)
    float BaseValue = 0.f;

    UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Attribute)
    float MinValue = 0.f;

    UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Attribute)
    float MaxValue = 0.f;

    UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Attribute)
    FString DerivedAttributeInfo;

    UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Attribute)
    bool bCanStack = false;
};

// Transient form data. Asset changes happen only on an explicit Create / Save action.
UCLASS(Transient)
class TOWERDEFENSEUNITEDITOR_API UUnitEditorModel : public UObject
{
    GENERATED_BODY()

public:
    UUnitEditorModel();
    bool LoadUnit(UBlueprint* Blueprint, FText& OutError);
    bool Validate(FText& OutError) const;
    bool ApplyTo(UBlueprint* Blueprint, UDataTable* Table, FText& OutError);
    static bool MakeAssetPaths(const FString& Name, EUnitEditorTeam UnitTeam, FString& OutBlueprintPath, FString& OutTablePath, FText& OutError);
    void ResetAttributes();

    UPROPERTY(EditAnywhere, Category = "01 Source", meta = (DisplayName = "Unit / template Blueprint"))
    TObjectPtr<UBlueprint> SourceUnit;

    UPROPERTY(VisibleAnywhere, Category = "01 Source")
    TObjectPtr<UBlueprint> LoadedUnit;

    UPROPERTY(VisibleAnywhere, Category = "02 Creation")
    TObjectPtr<UBlueprint> CreationTemplate;

    UPROPERTY(VisibleAnywhere,
              Category = "01 Source",
              meta = (ToolTip = "Read from the loaded unit's AbilitySystemComponent. Saving updates this table, including when other units share it."))
    TObjectPtr<UDataTable> AttributeTable;

    UPROPERTY(EditAnywhere, Category = "02 Creation", meta = (ToolTip = "Name without BP_ / DT_ prefixes. Used only when creating a new unit."))
    FString UnitName = TEXT("NewUnit");

    UPROPERTY(EditAnywhere,
              Category = "02 Creation",
              meta = (AllowAbstract = "false", ToolTip = "Used by New blank. For ranged units choose AIShootingCharacter or a suitable Blueprint parent."))
    TSubclassOf<ATowerDefenseAICharacter> CreationParentClass;

    UPROPERTY(EditAnywhere, Category = "03 Unit")
    EUnitEditorTeam Team = EUnitEditorTeam::Defender;

    UPROPERTY(EditAnywhere, Category = "03 Unit")
    EUnitAttackPattern AttackPattern = EUnitAttackPattern::Adjacent;

    UPROPERTY(EditAnywhere, Category = "03 Unit", meta = (ClampMin = "0", Units = "cm", DisplayName = "Attack range (cm)"))
    float AttackRange = 0.f;

    UPROPERTY(EditAnywhere, Category = "03 Unit", meta = (ClampMin = "1", ClampMax = "20", DisplayName = "Attack radius (cells)"))
    int32 AttackRadiusInCells = 1;

    UPROPERTY(EditAnywhere, Category = "03 Unit", meta = (AllowAbstract = "false"))
    TSubclassOf<AAIController> ControllerClass;

    UPROPERTY(EditAnywhere, Category = "04 Visuals")
    TObjectPtr<USkeletalMesh> SkeletalMesh;

    UPROPERTY(EditAnywhere, Category = "04 Visuals", meta = (AllowAbstract = "false"))
    TSubclassOf<UAnimInstance> AnimationClass;

    UPROPERTY(EditAnywhere, Category = "04 Visuals")
    FTransform MeshTransform;

    UPROPERTY(EditAnywhere, EditFixedSize, Category = "05 Attributes", meta = (TitleProperty = "Name", EditFixedOrder))
    TArray<FUnitEditorAttribute> Attributes;

    UPROPERTY(EditAnywhere, Category = "06 Abilities", meta = (TitleProperty = "Ability"))
    TArray<FDefaultAbilityDescription> DefaultAbilities;

    UPROPERTY(EditAnywhere, Category = "07 Shooting", meta = (EditCondition = "AttackPattern == EUnitAttackPattern::Ranged"))
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "07 Shooting", meta = (EditCondition = "AttackPattern == EUnitAttackPattern::Ranged"))
    FName SpawnSocketName = TEXT("Weapon_Socket_L");

    UPROPERTY(EditAnywhere, Category = "07 Shooting", meta = (EditCondition = "AttackPattern == EUnitAttackPattern::Ranged"))
    FName TargetBlackboardKey = TEXT("AttackTarget");

private:
    void ReadCharacterSettings(ATowerDefenseAICharacter* Unit);
    void ReadAttributeTable(const UDataTable* Table);
    bool AreAbilitiesValid(FText& OutError) const;
    bool AreAttributesValid(FText& OutError) const;
    void ApplyCharacterSettings(ATowerDefenseAICharacter* Unit) const;
    void ApplyAttributeTable(UAbilitySystemComponent* AbilitySystem, UDataTable* Table) const;
};
