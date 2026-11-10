// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class SToolTip;

class FEquipmentDefinitionDetails : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance();
    virtual void CustomizeDetails(IDetailLayoutBuilder& Builder) override;

private:
    static void GetAttachmentOptions(TArray<TSharedPtr<FString>>& Options, TArray<TSharedPtr<SToolTip>>& Tooltips, TArray<bool>& Restricted);
};
