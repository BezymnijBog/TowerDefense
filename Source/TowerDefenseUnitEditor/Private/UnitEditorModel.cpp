// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "UnitEditorModel.h"

#include "Actors/Projectile.h"
#include "AI/AIShootingCharacter.h"
#include "AI/DefenderCharacter.h"
#include "AIController.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "Animation/Skeleton.h"
#include "Attributes/AICharacterAttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/DataTable.h"
#include "Engine/SkeletalMesh.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "UObject/UnrealType.h"
#include "Utils/BaseUtils.h"

namespace
{
constexpr int32 AmbiguousAttributeDefaults = -2;

template <typename PropertyType>
PropertyType& GetPropertyValue(UObject* Object, const FName Name)
{
    FProperty* Property = FindFProperty<FProperty>(Object->GetClass(), Name);
    checkf(Property, TEXT("Missing unit editor property %s"), *Name.ToString());
    return *Property->ContainerPtrToValuePtr<PropertyType>(Object);
}

int32 FindAttributeDefaults(const UAbilitySystemComponent* AbilitySystem)
{
    int32 Index = INDEX_NONE;
    for (int32 EntryIndex = 0; EntryIndex < AbilitySystem->DefaultStartingData.Num(); ++EntryIndex)
    {
        const FAttributeDefaults& Entry = AbilitySystem->DefaultStartingData[EntryIndex];
        if (Entry.Attributes && Entry.Attributes->IsChildOf(UAICharacterAttributeSet::StaticClass()))
        {
            if (Index != INDEX_NONE)
            {
                return AmbiguousAttributeDefaults;
            }
            Index = EntryIndex;
        }
    }
    return Index;
}

FName MakeAttributeRowName(FName Attribute)
{
    return FName(*(UAICharacterAttributeSet::StaticClass()->GetName() + TEXT(".") + Attribute.ToString()));
}
} // namespace

UUnitEditorModel::UUnitEditorModel()
{
    CreationParentClass = ADefenderCharacter::StaticClass();
    MeshTransform = FTransform(FRotator(0., -90., 0.), FVector(0., 0., -90.));
    ResetAttributes();
}

void UUnitEditorModel::ResetAttributes()
{
    Attributes.Reset();
    for (TFieldIterator<FStructProperty> It(UAICharacterAttributeSet::StaticClass()); It; ++It)
    {
        if (It->Struct != FGameplayAttributeData::StaticStruct())
        {
            continue;
        }
        FUnitEditorAttribute& Attribute = Attributes.AddDefaulted_GetRef();
        Attribute.Name = It->GetFName();
        if (Attribute.Name == TEXT("Health") || Attribute.Name == TEXT("MaxHealth"))
        {
            Attribute.BaseValue = 100.f;
        }
        else if (Attribute.Name == TEXT("Damage"))
        {
            Attribute.BaseValue = 10.f;
        }
        else if (Attribute.Name == TEXT("Cooldown"))
        {
            Attribute.BaseValue = 1.f;
        }
    }
}

bool UUnitEditorModel::LoadUnit(UBlueprint* Blueprint, FText& OutError)
{
    ATowerDefenseAICharacter* Unit =
        IsValid(Blueprint) && Blueprint->GeneratedClass ? Cast<ATowerDefenseAICharacter>(Blueprint->GeneratedClass->GetDefaultObject()) : nullptr;
    if (!IsValid(Unit) || Blueprint->Status == BS_Error)
    {
        OutError = FText::FromString(TEXT("Select a compiled Blueprint derived from TowerDefenseAICharacter."));
        return false;
    }
    UAbilitySystemComponent* AbilitySystem = Unit->GetAbilitySystemComponent();
    if (!IsValid(AbilitySystem) || !IsValid(Unit->GetMesh()))
    {
        OutError = FText::FromString(TEXT("The unit must have AbilitySystem and CharacterMesh components."));
        return false;
    }
    const int32 Index = FindAttributeDefaults(AbilitySystem);
    if (Index == AmbiguousAttributeDefaults)
    {
        OutError = FText::FromString(
            TEXT("Several AICharacterAttributeSet entries found in DefaultStartingData. Select a single attribute set in the Blueprint first."));
        return false;
    }
    UDataTable* Table = Index >= 0 ? AbilitySystem->DefaultStartingData[Index].DefaultStartingTable : nullptr;
    if (Table && Table->GetRowStruct() != FAttributeMetaData::StaticStruct())
    {
        OutError = FText::FromString(TEXT("The attribute table must use AttributeMetaData rows."));
        return false;
    }
    const uint8 TeamId = Unit->GetGenericTeamId().GetId();
    if (TeamId != Teams::TeamPlayer && TeamId != Teams::TeamEnemy)
    {
        OutError = FText::FromString(TEXT("Only Defender (0) and Enemy (1) teams are supported."));
        return false;
    }
    SourceUnit = Blueprint;
    LoadedUnit = Blueprint;
    CreationTemplate = nullptr;
    AttributeTable = Table;
    CreationParentClass = Blueprint->ParentClass;
    UnitName = Blueprint->GetName();
    UnitName.RemoveFromStart(TEXT("BP_"));
    if (!UnitName.RemoveFromStart(TEXT("Enemy_")))
    {
        UnitName.RemoveFromStart(TEXT("Defender_"));
    }
    Team = TeamId == Teams::TeamPlayer ? EUnitEditorTeam::Defender : EUnitEditorTeam::Enemy;
    ReadCharacterSettings(Unit);
    ReadAttributeTable(Table);
    return true;
}

bool UUnitEditorModel::MakeAssetPaths(const FString& Name, EUnitEditorTeam UnitTeam, FString& OutBlueprintPath, FString& OutTablePath, FText& OutError)
{
    if (Name.IsEmpty() || !(FChar::IsAlpha(Name[0]) || Name[0] == TEXT('_')))
    {
        OutError = FText::FromString(TEXT("UnitName must start with a letter or underscore."));
        return false;
    }
    for (TCHAR Character : Name)
    {
        if (!FChar::IsAlnum(Character) && Character != TEXT('_'))
        {
            OutError = FText::FromString(TEXT("UnitName may contain only letters, digits and underscores."));
            return false;
        }
    }
    const FString Side = UnitTeam == EUnitEditorTeam::Enemy ? TEXT("Enemy") : TEXT("Defender");
    OutBlueprintPath = TEXT("/Game/TowerDefense/Core/Actors/") + (UnitTeam == EUnitEditorTeam::Enemy ? FString(TEXT("Enemy")) : FString(TEXT("Defenders")))
        + TEXT("/BP_") + Side + TEXT("_") + Name;
    OutTablePath = TEXT("/Game/TowerDefense/Core/Miscellaneous/DefaultAttributes/") + Side + TEXT("/DT_") + Side + TEXT("Attributes_") + Name;
    return FPackageName::IsValidLongPackageName(OutBlueprintPath, false, &OutError) && FPackageName::IsValidLongPackageName(OutTablePath, false, &OutError);
}

bool UUnitEditorModel::Validate(FText& OutError) const
{
    UClass* UnitClass = LoadedUnit ? LoadedUnit->GeneratedClass.Get() : CreationTemplate ? CreationTemplate->GeneratedClass.Get() : CreationParentClass.Get();
    if (!UnitClass || !UnitClass->IsChildOf(ATowerDefenseAICharacter::StaticClass()) || UnitClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated))
    {
        OutError = FText::FromString(TEXT("Choose a non-abstract character parent or load a unit template."));
        return false;
    }
    if (!SkeletalMesh || !AnimationClass || !ControllerClass)
    {
        OutError = FText::FromString(TEXT("Select a skeletal mesh, animation class and AI controller."));
        return false;
    }
    const UAnimBlueprintGeneratedClass* AnimClass = Cast<UAnimBlueprintGeneratedClass>(AnimationClass.Get());
    if (AnimClass && AnimClass->GetTargetSkeleton() && !AnimClass->GetTargetSkeleton()->IsCompatibleForEditor(SkeletalMesh->GetSkeleton()))
    {
        OutError = FText::FromString(TEXT("The animation Blueprint and mesh must use compatible skeletons."));
        return false;
    }
    if (!FMath::IsFinite(AttackRange) || AttackRange < 0.f || AttackRadiusInCells < 1 || AttackRadiusInCells > 20)
    {
        OutError = FText::FromString(TEXT("Attack range must be non-negative; cell radius must be between 1 and 20."));
        return false;
    }
    if (AttackPattern == EUnitAttackPattern::Ranged
        && (!UnitClass->IsChildOf(AAIShootingCharacter::StaticClass()) || !ProjectileClass || SpawnSocketName.IsNone() || TargetBlackboardKey.IsNone()))
    {
        OutError = FText::FromString(TEXT("Ranged units require an AIShootingCharacter parent, projectile class, spawn socket and target Blackboard key."));
        return false;
    }
    return AreAbilitiesValid(OutError) && AreAttributesValid(OutError);
}

bool UUnitEditorModel::AreAttributesValid(FText& OutError) const
{
    TSet<FName> Names;
    for (const FUnitEditorAttribute& Attribute : Attributes)
    {
        if (!FindFProperty<FStructProperty>(UAICharacterAttributeSet::StaticClass(), Attribute.Name) || Names.Contains(Attribute.Name)
            || !FMath::IsFinite(Attribute.BaseValue) || !FMath::IsFinite(Attribute.MinValue) || !FMath::IsFinite(Attribute.MaxValue))
        {
            OutError = FText::FromString(TEXT("Attribute rows must be unique, valid and contain finite values."));
            return false;
        }
        Names.Add(Attribute.Name);
    }
    for (TFieldIterator<FStructProperty> It(UAICharacterAttributeSet::StaticClass()); It; ++It)
    {
        if (It->Struct == FGameplayAttributeData::StaticStruct() && !Names.Contains(It->GetFName()))
        {
            OutError = FText::FromString(TEXT("All character attributes must be present. Reload the unit to restore deleted rows."));
            return false;
        }
    }
    return true;
}

bool UUnitEditorModel::ApplyTo(UBlueprint* Blueprint, UDataTable* Table, FText& OutError)
{
    if (!Validate(OutError))
    {
        return false;
    }
    ATowerDefenseAICharacter* Unit =
        Blueprint && Blueprint->GeneratedClass ? Cast<ATowerDefenseAICharacter>(Blueprint->GeneratedClass->GetDefaultObject()) : nullptr;
    UAbilitySystemComponent* AbilitySystem = Unit ? Unit->GetAbilitySystemComponent() : nullptr;
    if (!Unit || !AbilitySystem || !Table || Table->GetRowStruct() != FAttributeMetaData::StaticStruct()
        || FindAttributeDefaults(AbilitySystem) == AmbiguousAttributeDefaults)
    {
        OutError = FText::FromString(TEXT("Invalid character, AbilitySystem or attribute table."));
        return false;
    }
    Blueprint->Modify();
    Unit->Modify();
    AbilitySystem->Modify();
    Unit->GetMesh()->Modify();
    Table->Modify();
    ApplyCharacterSettings(Unit);
    ApplyAttributeTable(AbilitySystem, Table);
    FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
    FKismetEditorUtilities::CompileBlueprint(Blueprint);
    if (Blueprint->Status == BS_Error)
    {
        OutError = FText::FromString(TEXT("Blueprint compilation failed. Changes remain unsaved; inspect the compiler log."));
        return false;
    }
    return true;
}

void UUnitEditorModel::ReadCharacterSettings(ATowerDefenseAICharacter* Unit)
{
    AttackPattern = GetPropertyValue<EUnitAttackPattern>(Unit, TEXT("AttackPattern"));
    AttackRange = GetPropertyValue<float>(Unit, TEXT("AttackRange"));
    AttackRadiusInCells = GetPropertyValue<int32>(Unit, TEXT("AttackRadiusInCells"));
    ControllerClass = Unit->AIControllerClass;
    DefaultAbilities = GetPropertyValue<TArray<FDefaultAbilityDescription>>(Unit, TEXT("DefaultAbilities"));
    SkeletalMesh = Unit->GetMesh()->GetSkeletalMeshAsset();
    AnimationClass = Unit->GetMesh()->GetAnimClass();
    MeshTransform = Unit->GetMesh()->GetRelativeTransform();
    ProjectileClass = nullptr;
    if (Unit->IsA<AAIShootingCharacter>())
    {
        ProjectileClass = GetPropertyValue<TSubclassOf<AProjectile>>(Unit, TEXT("ProjectileClass"));
        SpawnSocketName = GetPropertyValue<FName>(Unit, TEXT("SpawnSocketName"));
        TargetBlackboardKey = GetPropertyValue<FName>(Unit, TEXT("TargetBlackboardKey"));
    }
}

void UUnitEditorModel::ReadAttributeTable(const UDataTable* Table)
{
    ResetAttributes();
    for (FUnitEditorAttribute& Attribute : Attributes)
    {
        const FAttributeMetaData* Row = Table ? Table->FindRow<FAttributeMetaData>(MakeAttributeRowName(Attribute.Name), TEXT("UnitEditor"), false) : nullptr;
        Attribute.BaseValue = Row ? Row->BaseValue : 0.f;
        if (Row)
        {
            Attribute.MinValue = Row->MinValue;
            Attribute.MaxValue = Row->MaxValue;
            Attribute.DerivedAttributeInfo = Row->DerivedAttributeInfo;
            Attribute.bCanStack = Row->bCanStack;
        }
    }
}

bool UUnitEditorModel::AreAbilitiesValid(FText& OutError) const
{
    for (const FDefaultAbilityDescription& Ability : DefaultAbilities)
    {
        if (!Ability.Ability || Ability.Ability->HasAnyClassFlags(CLASS_Abstract) || Ability.Level < 1)
        {
            OutError = FText::FromString(TEXT("Every ability needs a non-abstract class and a level of at least 1."));
            return false;
        }
    }
    return true;
}

void UUnitEditorModel::ApplyCharacterSettings(ATowerDefenseAICharacter* Unit) const
{
    Unit->SetGenericTeamId(Team == EUnitEditorTeam::Defender ? Teams::TeamPlayer : Teams::TeamEnemy);
    Unit->AIControllerClass = ControllerClass;
    Unit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    GetPropertyValue<EUnitAttackPattern>(Unit, TEXT("AttackPattern")) = AttackPattern;
    GetPropertyValue<float>(Unit, TEXT("AttackRange")) = AttackRange;
    GetPropertyValue<int32>(Unit, TEXT("AttackRadiusInCells")) = AttackRadiusInCells;
    GetPropertyValue<TArray<FDefaultAbilityDescription>>(Unit, TEXT("DefaultAbilities")) = DefaultAbilities;
    if (FBoolProperty* Stationary = FindFProperty<FBoolProperty>(Unit->GetClass(), TEXT("bOccupiesStationaryGridCells")))
    {
        Stationary->SetPropertyValue_InContainer(Unit, Team == EUnitEditorTeam::Defender);
    }
    Unit->GetMesh()->SetSkeletalMeshAsset(SkeletalMesh);
    Unit->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    Unit->GetMesh()->SetAnimInstanceClass(AnimationClass);
    Unit->GetMesh()->SetRelativeTransform(MeshTransform);
    if (Unit->IsA<AAIShootingCharacter>())
    {
        GetPropertyValue<TSubclassOf<AProjectile>>(Unit, TEXT("ProjectileClass")) = ProjectileClass;
        GetPropertyValue<FName>(Unit, TEXT("SpawnSocketName")) = SpawnSocketName;
        GetPropertyValue<FName>(Unit, TEXT("TargetBlackboardKey")) = TargetBlackboardKey;
    }
}

void UUnitEditorModel::ApplyAttributeTable(UAbilitySystemComponent* AbilitySystem, UDataTable* Table) const
{
    int32 Index = FindAttributeDefaults(AbilitySystem);
    if (Index == INDEX_NONE)
    {
        Index = AbilitySystem->DefaultStartingData.AddDefaulted();
        AbilitySystem->DefaultStartingData[Index].Attributes = UAICharacterAttributeSet::StaticClass();
    }
    AbilitySystem->DefaultStartingData[Index].DefaultStartingTable = Table;
    for (const FUnitEditorAttribute& Attribute : Attributes)
    {
        FAttributeMetaData Row;
        Row.BaseValue = Attribute.BaseValue;
        Row.MinValue = Attribute.MinValue;
        Row.MaxValue = Attribute.MaxValue;
        Row.DerivedAttributeInfo = Attribute.DerivedAttributeInfo;
        Row.bCanStack = Attribute.bCanStack;
        Table->AddRow(MakeAttributeRowName(Attribute.Name), Row);
    }
    std::ignore = Table->MarkPackageDirty();
}
