#include "Subsystems/GameStartSubsystem.h"

#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "Settings/TowerDefenseSettings.h"
#include "Styling/CoreStyle.h"
#include "TowerDefense.h"
#include "Widgets/Layout/SBorder.h"

void UGameStartSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnMapLoaded);
    if (IsValid(GEngine))
    {
        GEngine->OnTravelFailure().AddUObject(this, &ThisClass::OnTravelFailed);
    }
    TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::TickTransition));
}

bool UGameStartSubsystem::StartGame()
{
    if (State != Idle || !IsValid(GetGameInstance()))
    {
        return false;
    }
    const TSoftObjectPtr<UWorld>& Map = UTowerDefenseSettings::Get().GameMap;
    UGameViewportClient* GameViewport = GetGameInstance()->GetGameViewportClient();
    if (!IsValid(GameViewport) || Map.IsNull() || !FPackageName::DoesPackageExist(Map.ToSoftObjectPath().GetLongPackageName()))
    {
        UE_LOG(LogTowerDefense, Error, TEXT("Cannot start game: missing viewport or invalid GameMap."));
        return false;
    }
    Viewport = GameViewport;
    Opacity = 0.f;
    bFailedTransition = false;
    bMapReady = false;
    State = FadeOut;
    Overlay = SNew(SBorder)
                  .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                  .BorderBackgroundColor_Lambda([this]() { return FSlateColor(FLinearColor(0.f, 0.f, 0.f, Opacity)); })
                  .OnMouseButtonDown_Lambda([](const FGeometry&, const FPointerEvent&) { return FReply::Handled(); });
    GameViewport->AddViewportWidgetContent(Overlay.ToSharedRef(), 10000);
    LoadGameAssets();
    return true;
}

void UGameStartSubsystem::LoadGameAssets()
{
    if (bLoadingAssets)
    {
        return;
    }
    bAssetsReady = false;
    bLoadFailed = false;
    bLoadingAssets = true;
    RequestedAssets.Reset();
    OnCollectGameAssets.Broadcast(RequestedAssets);
    if (RequestedAssets.IsEmpty())
    {
        FinishAssetLoading();
        return;
    }
    // Treat unset required references as a failure, not an endless wait behind a black screen.
    for (const FSoftObjectPath& Path : RequestedAssets)
    {
        if (!Path.IsValid())
        {
            bLoadFailed = true;
        }
    }
    if (bLoadFailed)
    {
        FinishAssetLoading();
        return;
    }
    LoadHandle =
        UAssetManager::GetStreamableManager().RequestAsyncLoad(RequestedAssets, FStreamableDelegate::CreateUObject(this, &ThisClass::FinishAssetLoading));
    if (!LoadHandle.IsValid())
    {
        bLoadFailed = true;
        FinishAssetLoading();
    }
}

void UGameStartSubsystem::FinishAssetLoading()
{
    for (const FSoftObjectPath& Path : RequestedAssets)
    {
        if (!IsValid(Path.ResolveObject()))
        {
            UE_LOG(LogTowerDefense, Error, TEXT("Failed to load game asset: %s"), *Path.ToString());
            bLoadFailed = true;
        }
    }
    bLoadingAssets = false;
    bAssetsReady = !bLoadFailed;
    OnGameAssetsLoaded.Broadcast(bAssetsReady);
    // Subscribers have now stored their references. Keep the handle until the next request or shutdown.
}

bool UGameStartSubsystem::TickTransition(float DeltaTime)
{
    const UTowerDefenseSettings& Settings = UTowerDefenseSettings::Get();
    if (State == FadeOut)
    {
        if (bLoadFailed)
        {
            bFailedTransition = true;
            State = FadeIn;
            return true;
        }

        // Wait one tick at full black so Slate presents an opaque frame before synchronous map travel.
        if (Opacity >= 1.f && bAssetsReady)
        {
            State = WaitingForMap;
            UGameplayStatics::OpenLevelBySoftObjectPtr(this, Settings.GameMap);
            return true;
        }
        Opacity = Settings.GameFadeOutDuration > 0.f ? FMath::Min(1.f, Opacity + DeltaTime / Settings.GameFadeOutDuration) : 1.f;
    }
    else if (State == WaitingForMap && bMapReady)
    {
        if (const UWorld* World = GetWorld(); IsValid(World) && World->HasBegunPlay())
        {
            State = FadeIn;
        }
    }
    else if (State == FadeIn)
    {
        Opacity = Settings.GameFadeInDuration > 0.f ? FMath::Max(0.f, Opacity - DeltaTime / Settings.GameFadeInDuration) : 0.f;
        if (Opacity <= 0.f)
        {
            RemoveOverlay();
            State = Idle;
            if (bFailedTransition)
            {
                OnGameStartFailed.Broadcast();
            }
        }
    }
    return true;
}

void UGameStartSubsystem::OnMapLoaded(UWorld* World)
{
    if (State == WaitingForMap && IsValid(World) && World->GetGameInstance() == GetGameInstance())
    {
        bMapReady = true;
    }
}

void UGameStartSubsystem::OnTravelFailed(UWorld* World, ETravelFailure::Type, const FString& Error)
{
    if (State != Idle && IsValid(World) && World->GetGameInstance() == GetGameInstance())
    {
        UE_LOG(LogTowerDefense, Error, TEXT("Game travel failed: %s"), *Error);
        bFailedTransition = true;
        State = FadeIn;
    }
}

void UGameStartSubsystem::RemoveOverlay()
{
    if (Viewport.IsValid() && Overlay.IsValid())
    {
        Viewport->RemoveViewportWidgetContent(Overlay.ToSharedRef());
    }
    Overlay.Reset();
    Viewport.Reset();
}

void UGameStartSubsystem::Deinitialize()
{
    FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
    if (IsValid(GEngine))
    {
        GEngine->OnTravelFailure().RemoveAll(this);
    }
    if (LoadHandle.IsValid())
    {
        LoadHandle->CancelHandle();
        LoadHandle.Reset();
    }
    RemoveOverlay();
    OnCollectGameAssets.Clear();
    OnGameAssetsLoaded.Clear();
    OnGameStartFailed.Clear();
    Super::Deinitialize();
}
