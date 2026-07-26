// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/DefenderBase.h"

#include "AbilitySystemComponent.h"
#include "TowerDefense.h"
#include "Abilities/TowerDefenceTags.h"
#include "Actors/Projectile.h"
#include "Attributes/TurretAttributeSet.h"
#include "Components/BuildingComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ADefenderBase::ADefenderBase()
{
    PrimaryActorTick.bCanEverTick = true;

    TurretRoot = CreateDefaultSubobject<UBuildingComponent>(TEXT("Root"));
    SetRootComponent(TurretRoot);

    TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
    TurretMesh->SetupAttachment(TurretRoot);
    TurretMesh->SetVisibility(false);

    PreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
    PreviewMesh->SetupAttachment(TurretRoot);
    PreviewMesh->SetVisibility(true);

    ActiveVolume = CreateDefaultSubobject<USphereComponent>(TEXT("ShotVolume"));
    ActiveVolume->SetupAttachment(TurretRoot);

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

    TeamId.SetAttitudeSolver(Teams::AttitudeSolveFunction);
}

UAbilitySystemComponent* ADefenderBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ADefenderBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateTargets();
    if (!bIsIdle && AbilitySystemComponent->TryActivateAbility(ShotAbilityHandle))
    {
        MakeShot();
    }
}

FGenericTeamId ADefenderBase::GetGenericTeamId() const
{
    return TeamId;
}

void ADefenderBase::SetGenericTeamId(const FGenericTeamId& TeamID)
{
    TeamId = TeamID;
}

bool ADefenderBase::IsAllowedToBuild() const
{
    return bIsAllowedToBuild;
}

void ADefenderBase::UpdateIsBuildingAllowed()
{
    static constexpr float CollisionRadius = 40.f;
    const FCollisionShape Collision = FCollisionShape::MakeSphere(CollisionRadius);
    const FVector SweepStart = TurretMesh->GetComponentLocation();
    const FVector SweepEnd = SweepStart + FVector::UpVector * CollisionRadius * 2.;
    const FCollisionQueryParams Query(TEXT("BuildingUpdate"), false, this);

    FHitResult Hit;
    SetIsBuildingAllowed(!GetWorld()->SweepSingleByChannel(Hit, SweepStart, SweepEnd, FQuat::Identity, ECC_Visibility, Collision, Query) && !Hit.bBlockingHit);
}

void ADefenderBase::FinishBuilding() const
{
    PreviewMesh->SetVisibility(false);
    TurretMesh->SetVisibility(true);
    ActiveVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    ActiveVolume->OnComponentBeginOverlap.AddDynamic(this, &ADefenderBase::OnActiveVolumeBeginOverlap);
    ActiveVolume->OnComponentEndOverlap.AddDynamic(this, &ADefenderBase::OnActiveVolumeEndOverlap);

    TurretRoot->CanBuildChanged.Unbind();
}

void ADefenderBase::MakeShot()
{
    AProjectile* const Projectile = GetOrCreateProjectile();
    const FTransform Transform(UKismetMathLibrary::MakeRotFromXZ((CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal(), FVector::UpVector),
                               TurretMesh->GetSocketLocation(TEXT("Projectile")));
    Projectile->Activate(Transform, CurrentTarget.Get(), this);
    Projectile->OnHitDelegate.BindUObject(this, &ThisClass::OnProjectileHit);
}

void ADefenderBase::BeginPlay()
{
    Super::BeginPlay();

    TurretAttributes = AbilitySystemComponent->GetSet<UTurretAttributeSet>();
    check(IsValid(TurretAttributes));

    ShotAbilityHandle = AbilitySystemComponent->GiveAbility(ShotAbility);
    AbilitySystemComponent->AddLooseGameplayTag(Turret_State_Ready);

    TurretRoot->CanBuildChanged.BindUObject(this, &ThisClass::SetIsBuildingAllowed);
    ActiveVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADefenderBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ActiveVolume->OnComponentBeginOverlap.RemoveDynamic(this, &ADefenderBase::OnActiveVolumeBeginOverlap);
    ActiveVolume->OnComponentEndOverlap.RemoveDynamic(this, &ADefenderBase::OnActiveVolumeEndOverlap);

    Super::EndPlay(EndPlayReason);
}

void ADefenderBase::OnActiveVolumeBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (!IsValid(OtherActor) || !OtherActor->Implements<UGenericTeamAgentInterface>())
    {
        return;
    }

    if (GetTeamAttitudeTowards(*OtherActor) == ETeamAttitude::Hostile)
    {
        UE_LOG(LogTowerDefense, Display, TEXT("%s entered %s shoot zone"), *OtherActor->GetName(), *GetName());
        AddTarget(Cast<AEnemyCharacter>(OtherActor));
    }
    bIsIdle = !Targets.IsEmpty();
}

void ADefenderBase::OnActiveVolumeEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
    if (!IsValid(OtherActor) || !OtherActor->Implements<UGenericTeamAgentInterface>())
    {
        return;
    }

    UE_LOG(LogTowerDefense, Display, TEXT("%s left %s shoot zone"), *OtherActor->GetName(), *GetName());
    RemoveTarget(Cast<AEnemyCharacter>(OtherActor));
    bIsIdle = Targets.IsEmpty();
}

void ADefenderBase::AddTarget(AEnemyCharacter* Enemy)
{
    if (!IsValid(Enemy))
    {
        return;
    }

    if (!Targets.Contains(Enemy))
    {
        TargetDelegates.Emplace(Enemy->OnDeathDelegate.AddUObject(this, &ThisClass::RemoveTarget));
        Targets.Emplace(Enemy);
    }
}

void ADefenderBase::OnIsBuildingAllowedChanged()
{
    UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(PreviewMaterial, this);
    const FColor& HighlightColor = bIsAllowedToBuild ? AllowBuildColor : ForbiddenBuildColor;
    MaterialInstance->SetVectorParameterValue(TEXT("Color"), FLinearColor::FromSRGBColor(HighlightColor));
    PreviewMesh->SetMaterial(0, MaterialInstance);
}

void ADefenderBase::OnProjectileHit(AProjectile* Projectile)
{
    Projectile->Deactivate();
    Projectiles.Emplace(Projectile);
}

void ADefenderBase::RemoveTarget(AEnemyCharacter* Enemy)
{
    if (!IsValid(Enemy))
    {
        return;
    }

    if (const int32 Idx = Targets.IndexOfByKey(Enemy); Idx != INDEX_NONE)
    {
        Enemy->OnDeathDelegate.Remove(TargetDelegates[Idx]);
        TargetDelegates.RemoveAtSwap(Idx);
        Targets.RemoveAtSwap(Idx);
    }
}

void ADefenderBase::SetIsBuildingAllowed(bool bValue)
{
    if (bIsAllowedToBuild != bValue)
    {
        bIsAllowedToBuild = bValue;
        OnIsBuildingAllowedChanged();
    }
}

void ADefenderBase::UpdateTargets()
{
    check(TargetDelegates.Num() == Targets.Num());
    bIsIdle = Targets.IsEmpty();
    CurrentTarget = bIsIdle ? nullptr : Targets[0];
}

AProjectile* ADefenderBase::GetOrCreateProjectile()
{
    return Projectiles.IsEmpty() ? GetWorld()->SpawnActor<AProjectile>(ProjectileClass) : Projectiles.Pop();
}
