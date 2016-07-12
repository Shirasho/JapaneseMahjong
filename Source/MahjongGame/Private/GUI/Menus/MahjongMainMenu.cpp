// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongMainMenu.h"

#include "GUIDefinitions.h"

#include "MahjongGameUserSettings.h"
#include "MahjongGameInstance.h"

#include "SMahjongMenuWidget.h"
#include "SMahjongDialogWidget.h"

#include "GenericPlatformChunkInstall.h"

//use an EMap index, get back the ChunkIndex that map should be part of.
//Instead of this mapping we should really use the AssetRegistry to query for chunk mappings, but maps aren't members of the AssetRegistry yet.
static const int ChunkMapping[] = { 1, 2 };

FMahjongMainMenu::~FMahjongMainMenu()
{
    IOnlineSessionPtr Sessions = Online::GetSessionInterface();
    CleanupOnlinePrivilegeTask();
}

void FMahjongMainMenu::Construct(TWeakObjectPtr<UMahjongGameInstance> InGameInstance, TWeakObjectPtr<ULocalPlayer> InPlayerOwner)
{
    bShowingDownloadPct = false;
    bAnimateSearchingUI = false;
    bUsedInputToCancelSearch = false;
    bSearchRequestCancelled = false;
    bIncrementAlpha = false;

    check(InGameInstance.IsValid());
    IOnlineSessionPtr Sessions = Online::GetSessionInterface();

    PlayerOwner = InPlayerOwner;
    GameInstance = InGameInstance;

    OnMatchmakingCompleteDelegate = FOnMatchmakingCompleteDelegate::CreateSP(this, &FMahjongMainMenu::OnMatchmakingComplete);
    OnCancelMatchmakingCompleteDelegate = FOnCancelMatchmakingCompleteDelegate::CreateSP(this, &FMahjongMainMenu::OnCancelMatchmakingComplete);

#if MAHJONG_CONSOLE_UI
    bIsLanMatch = false;
#else
    UMahjongGameUserSettings* UserSettings = CastChecked<UMahjongGameUserSettings>(GEngine->GetGameUserSettings());
    bIsLanMatch = UserSettings->IsLanMatch();
#endif

    //@TODO Build the menu
}

void FMahjongMainMenu::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
    if (GameInstance.IsValid())
    {
        // Lock controls for the duration of the async task
        MenuWidget->LockControls(true);
        TSharedPtr<const FUniqueNetId> UserId;
        if (PlayerOwner.IsValid())
        {
            UserId = PlayerOwner->GetPreferredUniqueNetId();
        }
        GameInstance->StartOnlinePrivilegeTask(Delegate, EUserPrivileges::CanPlayOnline, UserId);
    }
}

void FMahjongMainMenu::CleanupOnlinePrivilegeTask()
{
    if (GameInstance.IsValid())
    {
        GameInstance->CleanupOnlinePrivilegeTask();
    }
}

void FMahjongMainMenu::OnMatchmakingComplete(FName SessionName, bool bWasSuccessful)
{
    IOnlineSessionPtr SessionInterface = Online::GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogOnline, Warning, TEXT("OnMatchmakingComplete(): Couldn't find session interface."));
        return;
    }

    SessionInterface->ClearOnMatchmakingCompleteDelegate_Handle(OnMatchmakingCompleteDelegateHandle);

    if (bSearchRequestCancelled && bUsedInputToCancelSearch)
    {
        bSearchRequestCancelled = false;
        // Clean up the session in case we get this event after cancelling.
        if (bWasSuccessful && SessionInterface.IsValid() && PlayerOwner.IsValid() && PlayerOwner->GetPreferredUniqueNetId().IsValid())
        {
            SessionInterface->DestroySession(GameSessionName);
        }

        return;
    }

    if (bAnimateSearchingUI)
    {
        bAnimateSearchingUI = false;
        MatchmakingSearchingUICancel(false);
        bUsedInputToCancelSearch = false;
    }
    else
    {
        return;
    }

    if (!bWasSuccessful)
    {
        UE_LOG(LogOnline, Warning, TEXT("Matchmaking was unsuccessful."));
        //@TODO
        //DisplayMatchmakingFailureUI();
        return;
    }

    UE_LOG(LogOnline, Log, TEXT("Matchmaking successful! Session name is %s."), *SessionName.ToString());

    if (GetPlayerOwner() == nullptr)
    {
        UE_LOG(LogOnline, Warning, TEXT("OnMatchmakingComplete: No owner."));
        return;
    }

    FNamedOnlineSession* MatchSession = SessionInterface->GetNamedSession(SessionName);
    if (!MatchSession)
    {
        UE_LOG(LogOnline, Warning, TEXT("OnMatchmakingComplete: No session."));
        return;
    }

    if (!MatchSession->OwningUserId.IsValid())
    {
        UE_LOG(LogOnline, Warning, TEXT("OnMatchmakingComplete: No session owner/host."));
        return;
    }

    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(SearchingWidgetContainer.ToSharedRef());
    }

    bAnimateSearchingUI = false;

    UE_LOG(LogOnline, Log, TEXT("OnMatchmakingComplete: Session host is %d."), *MatchSession->OwningUserId->ToString());

    if (ensure(GameInstance.IsValid()))
    {
        MenuWidget->LockControls(true);

        IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
        if (OnlineSubsystem && OnlineSubsystem->IsLocalPlayer(*MatchSession->OwningUserId))
        {
            // This console is the host - start the match.
            GameInstance->BeginHostingGame((int32)GetSelectedMap());
        }
        else
        {
            // We are a client - join the host.
            GameInstance->TravelToSession(SessionName);
        }
    }
}

void FMahjongMainMenu::OnCancelMatchmakingComplete(FName SessionName, bool bWasSuccessful)
{
    IOnlineSessionPtr SessionInterface = Online::GetSessionInterface();
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnCancelMatchmakingCompleteDelegate_Handle(OnCancelMatchmakingCompleteDelegateHandle);
    }

    bAnimateSearchingUI = false;
    
    UGameViewportClient* const GVC = GEngine->GameViewport;
    GVC->RemoveViewportWidgetContent(StoppingWidgetContainer.ToSharedRef());
    AddMenuToGameViewport();
    FSlateApplication::Get().SetKeyboardFocus(MenuWidget);
}

void FMahjongMainMenu::MatchmakingSearchingUICancel(bool bShouldRemoveSession)
{
    IOnlineSessionPtr SessionInterface = Online::GetSessionInterface();
    if (bShouldRemoveSession && SessionInterface.IsValid())
    {
        if (PlayerOwner.IsValid() && PlayerOwner->GetPreferredUniqueNetId().IsValid())
        {
            UGameViewportClient* const GVC = GEngine->GameViewport;
            GVC->RemoveViewportWidgetContent(SearchingWidgetContainer.ToSharedRef());
            GVC->AddViewportWidgetContent(StoppingWidgetContainer.ToSharedRef());
            FSlateApplication::Get().SetKeyboardFocus(StoppingWidgetContainer);

            OnCancelMatchmakingCompleteDelegateHandle = SessionInterface->AddOnCancelMatchmakingCompleteDelegate_Handle(OnCancelMatchmakingCompleteDelegate);
            SessionInterface->CancelMatchmaking(*PlayerOwner->GetPreferredUniqueNetId(), GameSessionName);
        }
    }
    else
    {
        UGameViewportClient* const GVC = GEngine->GameViewport;
        GVC->RemoveViewportWidgetContent(SearchingWidgetContainer.ToSharedRef());
        AddMenuToGameViewport();
        FSlateApplication::Get().SetKeyboardFocus(MenuWidget);
    }
}

void FMahjongMainMenu::AddMenuToGameViewport()
{
    if (GEngine && GEngine->GameViewport)
    {
        UGameViewportClient* const GVC = GEngine->GameViewport;
        GVC->AddViewportWidgetContent(MenuWidgetContainer.ToSharedRef());
    }
}

void FMahjongMainMenu::RemoveMenuFromGameViewport()
{
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(MenuWidgetContainer.ToSharedRef());
    }
}

void FMahjongMainMenu::Tick(float DeltaSeconds)
{
    if (bAnimateSearchingUI)
    {
        //@TODO
        /*FLinearColor SearchingColor = GameSearchingWidget->GetColorAndOpacity();
        if (bIncrementAlpha)
        {
            if (SearchingColor.A >= 1.f)
            {
                bIncrementAlpha = false;
            }
            else
            {
                SearchingColor.A += DeltaSeconds;
            }
        }
        else
        {
            if (SearchingColor.A <= .1f)
            {
                bIncrementAlpha = true;
            }
            else
            {
                SearchingColor.A -= DeltaSeconds;
            }
        }
        GameSearchingWidget->SetColorAndOpacity(SearchingColor);
        GameStoppingWidget->SetColorAndOpacity(SearchingColor);*/
    }

    IPlatformChunkInstall* ChunkInstaller = FPlatformMisc::GetPlatformChunkInstall();
    if (ChunkInstaller)
    {
        EMap SelectedMap = GetSelectedMap();

        // Use the asset registry when maps are added to it.
        int32 MapChunk = ChunkMapping[(int32)SelectedMap];
        EChunkLocation::Type ChunkLocation = ChunkInstaller->GetChunkLocation(MapChunk);

        FText UpdatedText;
        bool bUpdateText = false;

        if (ChunkLocation == EChunkLocation::NotAvailable)
        {
            float PercentComplete = FMath::Min(ChunkInstaller->GetChunkProgress(MapChunk, EChunkProgressReportingType::PercentageComplete), 100.f);
            UpdatedText = FText::FromString(FString::Printf(TEXT("%s %4.0f%%"), *NSLOCTEXT("Mahjong.HUD.Menu", "SelectedLevel", "Map").ToString(), PercentComplete));
            bUpdateText = true;
            bShowingDownloadPct = true;
        }
        else if (bShowingDownloadPct)
        {
            UpdatedText = NSLOCTEXT("Mahjong.HUD.Menu", "SelectedLevel", "Map");
            bUpdateText = true;
            bShowingDownloadPct = false;
        }
        
        if (bUpdateText)
        {
            //@TODO
            /*if (GameInstance.IsValid() && GameInstance->GetIsOnline() && HostOnlineMapOption.IsValid())
            {
                HostOnlineMapOption->SetText(UpdatedText);
            }
            else if (HostOfflineMapOption.IsValid())
            {
                HostOfflineMapOption->SetText(UpdatedText);
            }*/
        }
    }
}

bool FMahjongMainMenu::IsTickable() const
{
    return true;
}

TStatId FMahjongMainMenu::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(FMahjongMainMenu, STATGROUP_Tickables);
}

bool FMahjongMainMenu::IsTickableWhenPaused() const
{
    return true;
}

FMahjongMainMenu::EMap FMahjongMainMenu::GetSelectedMap() const
{
    //@TODO
    /*if (GameInstance.IsValid() && GameInstance->GetIsOnline() && HostOnlineMapOption.IsValid())
    {
        return (EMap)HostOnlineMapOption->SelectedMultiChoice;
    }
    else if (HostOfflineMapOption.IsValid())
    {
        return (EMap)HostOfflineMapOption->SelectedMultiChoice;
    }*/

    return EMap::EBambooPark;
}

ULocalPlayer* FMahjongMainMenu::GetPlayerOwner() const
{
    return PlayerOwner.Get();
}