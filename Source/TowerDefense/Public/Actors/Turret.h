// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "Utils/BaseUtils.h"
#include "Turret.generated.h"

class AEnemyCharacter;
class AProjectile;
class UBuildingComponent;
class UGameplayAbility;
class UNiagaraSystem;
class UBoxComponent;
class USphereComponent;
class UTurretAttributeSet;

UCLASS()
class TOWERDEFENSE_API ATurret : public AActor, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    ATurret();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual void Tick(float DeltaTime) override;

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

    bool IsAllowedToBuild() const;
    void UpdateIsBuildingAllowed();
    void FinishBuilding() const;

    void MakeShot();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<const UTurretAttributeSet> TurretAttributes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBuildingComponent> TurretRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> TurretMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> PreviewMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> ActiveVolume;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret|State")
    bool bIsIdle = true;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret|State")
    bool bIsAllowedToBuild = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret|State")
    float TargetYaw = 0.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret|State")
    float TargetPitch = 0.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret|State")
    TArray<AActor*> Targets;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret|State")
    TWeakObjectPtr<AActor> CurrentTarget;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret|State")
    TArray<AProjectile*> Projectiles;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret|State")
    FGameplayAbilitySpecHandle ShotAbilityHandle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret|Settings")
    TSubclassOf<UGameplayAbility> ShotAbility;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret|Settings")
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret|Settings")
    float HorizontalAimingInterpSpeed = UE_HALF_PI;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret|Settings")
    TObjectPtr<UMaterialInterface> PreviewMaterial;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret|Settings")
    FColor AllowBuildColor = FColor(0, 255, 0, 128);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret|Settings")
    FColor ForbiddenBuildColor = FColor(255, 0, 0, 128);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret|Settings")
    FGenericTeamId TeamId = Teams::TeamPlayer;

    TArray<FDelegateHandle> TargetDelegates;

private:
    UFUNCTION()
    void OnActiveVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                    AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp,
                                    int32 OtherBodyIndex,
                                    bool bFromSweep,
                                    const FHitResult& SweepResult);
    UFUNCTION()
    void OnActiveVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void AddTarget(AEnemyCharacter* Enemy);
    void OnIsBuildingAllowedChanged();
    void OnProjectileHit(AProjectile* Projectile);
    void RemoveTarget(AEnemyCharacter* Enemy);
    void SetIsBuildingAllowed(bool bValue);
    void UpdateTargets();

    AProjectile* GetOrCreateProjectile();
};
