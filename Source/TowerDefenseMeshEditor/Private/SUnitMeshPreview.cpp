// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "SUnitMeshPreview.h"

#include "Components/SkeletalMeshComponent.h"
#include "EditorViewportClient.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/TextureCube.h"
#include "PreviewScene.h"

void SUnitMeshPreview::Construct(const FArguments&)
{
    Scene = MakeUnique<FPreviewScene>(FPreviewScene::ConstructionValues());
    Component = NewObject<USkeletalMeshComponent>();
    Scene->AddComponent(Component, FTransform::Identity);
    SEditorViewport::Construct(SEditorViewport::FArguments());
    ConfigureLighting();
}

SUnitMeshPreview::~SUnitMeshPreview()
{
    if (Client)
    {
        Client->Viewport = nullptr;
    }
    Client.Reset();
    Scene->RemoveComponent(Component);
}

TSharedRef<FEditorViewportClient> SUnitMeshPreview::MakeEditorViewportClient()
{
    TSharedRef<FEditorViewportClient> ViewportClient = MakeShared<FEditorViewportClient>(nullptr, Scene.Get(), SharedThis(this));
    ViewportClient->SetViewLocation(FVector(250.0, -250.0, 160.0));
    ViewportClient->SetViewRotation(FRotator(-15.0, 135.0, 0.0));
    ViewportClient->SetViewMode(VMI_Lit);
    ViewportClient->EngineShowFlags.SetGrid(true);
    return ViewportClient;
}

void SUnitMeshPreview::SetMesh(USkeletalMesh* Mesh)
{
    ConfigureLighting();
    Component->SetSkeletalMesh(Mesh);
    if (Mesh)
    {
        Client->FocusViewportOnBox(Mesh->GetBounds().GetBox(), true);
    }
    Invalidate();
}

void SUnitMeshPreview::ConfigureLighting() const
{
    constexpr float KeyLightBrightness = 2.0f;
    constexpr float FillLightBrightness = 1.0f;
    constexpr float PreviewExposureEV100 = 0.0f;
    Scene->SetLightBrightness(KeyLightBrightness);
    Scene->SetSkyBrightness(FillLightBrightness);
    Scene->SetSkyCubemap(LoadObject<UTextureCube>(nullptr, TEXT("/Engine/MapTemplates/Sky/DaylightAmbientCubemap.DaylightAmbientCubemap")));

    Client->ExposureSettings.bFixed = true;
    Client->ExposureSettings.FixedEV100 = PreviewExposureEV100;
    Client->EngineShowFlags.SetEyeAdaptation(false);
    Client->EngineShowFlags.SetBloom(false);
    Client->EngineShowFlags.SetLensFlares(false);
    Client->SetRealtime(true);
}
