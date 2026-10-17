#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Engine/EngineBaseTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameStartSubsystem.generated.h"

class SBorder;
struct FStreamableHandle;
class UGameViewportClient;

// Subscribers add required assets during collection, then retain their objects on completion.
DECLARE_MULTICAST_DELEGATE_OneParam(FCollectGameAssets, TArray<FSoftObjectPath>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FGameAssetsLoaded, bool);
DECLARE_MULTICAST_DELEGATE(FGameStartFailed);

UCLASS()
class TOWERDEFENSE_API UGameStartSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    FCollectGameAssets OnCollectGameAssets;
    FGameAssetsLoaded OnGameAssetsLoaded;
    FGameStartFailed OnGameStartFailed;

    bool StartGame();
    void LoadGameAssets();
    bool AreAssetsReady() const { return bAssetsReady; }

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    enum ETransitionState : uint8
    {
        Idle,
        FadeOut,
        WaitingForMap,
        FadeIn
    };

    bool TickTransition(float DeltaTime);
    void FinishAssetLoading();
    void OnMapLoaded(UWorld* World);
    void OnTravelFailed(UWorld* World, ETravelFailure::Type FailureType, const FString& Error);
    void RemoveOverlay();

    ETransitionState State = Idle;
    float Opacity = 0.f;
    bool bLoadingAssets = false;
    bool bAssetsReady = false;
    bool bLoadFailed = false;
    bool bMapReady = false;
    bool bFailedTransition = false;

    TArray<FSoftObjectPath> RequestedAssets;
    TSharedPtr<FStreamableHandle> LoadHandle;
    TSharedPtr<SBorder> Overlay;
    TWeakObjectPtr<UGameViewportClient> Viewport;
    FTSTicker::FDelegateHandle TickHandle;
};
