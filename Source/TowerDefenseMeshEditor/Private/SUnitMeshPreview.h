// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "SEditorViewport.h"

class FPreviewScene;
class USkeletalMesh;
class USkeletalMeshComponent;

class SUnitMeshPreview : public SEditorViewport
{
public:
    SLATE_BEGIN_ARGS(SUnitMeshPreview) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& Arguments);

    SUnitMeshPreview() = default;
    SUnitMeshPreview(const SUnitMeshPreview&) = delete;
    SUnitMeshPreview(SUnitMeshPreview&&) = delete;
    virtual ~SUnitMeshPreview() override;

    SUnitMeshPreview& operator=(const SUnitMeshPreview&) = delete;
    SUnitMeshPreview& operator=(SUnitMeshPreview&&) = delete;

    void SetMesh(USkeletalMesh* Mesh);

protected:
    virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

private:
    void ConfigureLighting() const;

    TUniquePtr<FPreviewScene> Scene;
    USkeletalMeshComponent* Component = nullptr;
};
