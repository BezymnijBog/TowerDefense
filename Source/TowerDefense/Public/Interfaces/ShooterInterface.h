

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ShooterInterface.generated.h"

class AProjectile;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UShooterInterface : public UInterface
{
    GENERATED_BODY()
};

class TOWERDEFENSE_API IShooterInterface : public IInterface
{
    GENERATED_BODY()

public:
    virtual void ActivateProjectile() = 0;
    virtual void LaunchActiveProjectile() = 0;
};
