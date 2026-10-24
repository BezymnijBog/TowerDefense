// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "Engine/SkeletalMesh.h"
#include "UnitMeshEditorModel.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/SCompoundWidget.h"

class IDetailsView;
class SUnitMeshPreview;
struct FPropertyChangedEvent;

class SUnitMeshEditor : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SUnitMeshEditor) {}
    SLATE_END_ARGS()
    void Construct(const FArguments& Arguments);
    bool CanClose() const;

private:
    FReply Preview();
    FReply Save();
    FReply LoadSelected();
    void OnChanged(const FPropertyChangedEvent& Event);
    TSharedRef<SWidget> MakeToolbar();
    TStrongObjectPtr<UUnitMeshEditorModel> Model;
    TStrongObjectPtr<USkeletalMesh> PendingSave;
    TSharedPtr<IDetailsView> Details;
    TSharedPtr<SUnitMeshPreview> Viewport;
    FText Status;
    bool bModified = false;
};
