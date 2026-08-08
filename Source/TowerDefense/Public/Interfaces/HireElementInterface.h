// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HireElementInterface.generated.h"

struct FHireInfo;

UINTERFACE(MinimalAPI)
class UHireElementInterface : public UInterface
{
    GENERATED_BODY()
};

class TOWERDEFENSE_API IHireElementInterface : public IInterface
{
    GENERATED_BODY()

public:
    virtual bool GetHireInfo(FHireInfo& Info);
};
