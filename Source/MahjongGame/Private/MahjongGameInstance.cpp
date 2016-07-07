// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameInstance.h"

#include "OnlineDefinitions.h"
#include "GUIDefinitions.h"

#include "MahjongGameState.h"
#include "MahjongGameSession.h"
#include "MahjongCharacter.h"
#include "MahjongPlayerController.h"
#include "MahjongPlayerController_Menu.h"

namespace MahjongGameInstanceState
{
    const FName None = FName(TEXT("None"));
    const FName PendingInvite = FName(TEXT("PendingInvite"));
    const FName WelcomeScreen = FName(TEXT("WelcomeScreen"));
    const FName MainMenu = FName(TEXT("MainMenu"));
    const FName MessageMenu = FName(TEXT("MessageMenu"));
    const FName Playing = FName(TEXT("Playing"));
}


UMahjongGameInstance::UMahjongGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bIsOnline(true) // Default to online
    , bIsLicensed(true) // Default to licensed (should be checked by online subsystem on boot)
{
    CurrentState = MahjongGameInstanceState::None;
}

void UMahjongGameInstance::Init()
{
    Super::Init();

    IgnorePairingChangeForControllerId = -1;
    CurrentConnectionStatus = EOnlineServerConnectionStatus::Connected;

#if MAHJONG_REQUIRE_OSS
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    check(OnlineSubsystem);
    IOnlineIdentityPtr OnlineIdentity = OnlineSubsystem->GetIdentityInterface();
    IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
    check(OnlineIdentity.IsValid());
    check(SessionInterface.IsValid());

    // Bind online subsystem delegates.
    // MAX_LOCAL_PLAYERS is 4 for consoles, 1 for everything else.
    for (int32 LocalPlayerIndex = 0; LocalPlayerIndex < MAX_LOCAL_PLAYERS; ++LocalPlayerIndex)
    {
        OnlineIdentity->AddOnLoginStatusChangedDelegate_Handle(LocalPlayerIndex, FOnLoginStatusChangedDelegate::CreateUObject(this, &UMahjongGameInstance::HandleUserLoginChanged));
    }

    OnlineIdentity->AddOnControllerPairingChangedDelegate_Handle(FOnControllerPairingChangedDelegate::CreateUObject(this, &UMahjongGameInstance::HandleControllerPairingChanged));
    OnlineSubsystem->AddOnConnectionStatusChangedDelegate_Handle(FOnConnectionStatusChangedDelegate::CreateUObject(this, &UMahjongGameInstance::HandleNetworkConnectionStatusChanged));
    SessionInterface->AddOnSessionFailureDelegate_Handle(FOnSessionFailureDelegate::CreateUObject(this, &UMahjongGameInstance::HandleSessionFailure));

    OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &UMahjongGameInstance::OnEndSessionComplete);
#endif

    // Bind core application delegates.
    FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &UMahjongGameInstance::HandleAppWillDeactivate);
    FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UMahjongGameInstance::HandleAppSuspend);
    FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UMahjongGameInstance::HandleAppResume);

    FCoreDelegates::OnSafeFrameChangedEvent.AddUObject(this, &UMahjongGameInstance::HandleSafeFrameChanged);
    FCoreDelegates::OnControllerConnectionChange.AddUObject(this, &UMahjongGameInstance::HandleControllerConnectionChange);
    FCoreDelegates::ApplicationLicenseChange.AddUObject(this, &UMahjongGameInstance::HandleAppLicenseUpdate);

    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UMahjongGameInstance::OnPreLoadMap);
    FCoreUObjectDelegates::PostLoadMap.AddUObject(this, &UMahjongGameInstance::OnPostLoadMap);

    TickDelegate = FTickerDelegate::CreateUObject(this, &UMahjongGameInstance::Tick);
    TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
}

void UMahjongGameInstance::Shutdown()
{
    Super::Shutdown();

    FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void UMahjongGameInstance::OnSessionUserInviteAccepted(
    const bool bWasSuccess,
    const int32 ControllerId,
    TSharedPtr<const FUniqueNetId> UserId,
    const FOnlineSessionSearchResult& InviteResult
)
{
    UE_LOG(LogOnline, Verbose, TEXT("HandleSessionUserInviteAccepted: bSuccess: %d, ControllerId: %d, User: %s"), bWasSuccess, ControllerId, UserId.IsValid() ? *UserId->ToString() : TEXT("nullptr "));

    if (!bWasSuccess)
    {
        return;
    }

    if (!InviteResult.IsValid())
    {
        UE_LOG(LogOnline, Warning, TEXT("Invite accept returned no search result."));
        return;
    }

    if (!UserId.IsValid())
    {
        UE_LOG(LogOnline, Warning, TEXT("Invite accept returned no user."));
        return;
    }

    // Set the pending invite, and then go to the initial screen, which is where we will process it
    PendingInvite.ControllerId = ControllerId;
    PendingInvite.UserId = UserId;
    PendingInvite.InviteResult = InviteResult;
    PendingInvite.bPrivilegesCheckedAndAllowed = false;

    GotoState(MahjongGameInstanceState::PendingInvite);
}

void UMahjongGameInstance::HandleNetworkConnectionStatusChanged(EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus)
{
    UE_LOG(LogOnlineGame, Warning, TEXT("UMahjongGameInstance::HandleNetworkConnectionStatusChanged: %s"), EOnlineServerConnectionStatus::ToString(ConnectionStatus));

#if MAHJONG_CONSOLE_UI
    // If we are disconnected from server, and not currently at (or heading to) the welcome screen
    // then display a message on consoles
    if (bIsOnline &&
        PendingState != MahjongGameInstanceState::WelcomeScreen &&
        CurrentState != MahjongGameInstanceState::WelcomeScreen &&
        ConnectionStatus != EOnlineServerConnectionStatus::Connected)
    {
        UE_LOG(LogOnlineGame, Log, TEXT("UMahjongGameInstance::HandleNetworkConnectionStatusChanged: Going to main menu"));

        // Display message on consoles
#if PLATFORM_XBOXONE
        const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection to Xbox LIVE has been lost.");
#elif PLATFORM_PS4
        const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection to PSN has been lost.");
#else
        const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection has been lost.");
#endif
        const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

        ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), MahjongGameInstanceState::MainMenu);
    }

    CurrentConnectionStatus = ConnectionStatus;
#endif
}

void UMahjongGameInstance::HandleSessionFailure(const FUniqueNetId& NetId, ESessionFailure::Type FailureType)
{
    UE_LOG(LogOnlineGame, Warning, TEXT("UMahjongGameInstance::HandleSessionFailure: %u"), (uint32)FailureType);

#if MAHJONG_CONSOLE_UI
    // If we are not currently at (or heading to) the welcome screen then display a message on consoles
    if (bIsOnline &&
        PendingState != MahjongGameInstanceState::WelcomeScreen &&
        CurrentState != MahjongGameInstanceState::WelcomeScreen)
    {
        UE_LOG(LogOnlineGame, Log, TEXT("UMahjongGameInstance::HandleSessionFailure: Going to main menu"));

        // Display message on consoles
#if PLATFORM_XBOXONE
        const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection to Xbox LIVE has been lost.");
#elif PLATFORM_PS4
        const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection to PSN has been lost.");
#else
        const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection has been lost.");
#endif
        const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

        ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), MahjongGameInstanceState::MainMenu);
    }
#endif
}

void UMahjongGameInstance::OnPreLoadMap()
{
    // Do nothing
}

void UMahjongGameInstance::OnPostLoadMap()
{
    // Make sure we hide the loading screen when the level is done loading
    //@TODO
    /*UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());

    if (MahjongViewport != nullptr)
    {
        MahjongViewport->HideLoadingScreen();
    }*/
}

void UMahjongGameInstance::OnUserCanPlayInvite(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
    CleanupOnlinePrivilegeTask();
    /*if (WelcomeMenuUI.IsValid())
    {
        WelcomeMenuUI->LockControls(false);
    }*/

    if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
    {
        if (UserId == *PendingInvite.UserId)
        {
            PendingInvite.bPrivilegesCheckedAndAllowed = true;
        }
    }
    else
    {
        DisplayOnlinePrivilegeFailureDialogs(UserId, Privilege, PrivilegeResults);
        GotoState(MahjongGameInstanceState::WelcomeScreen);
    }
}

void UMahjongGameInstance::StartGameInstance()
{
#if PLATFORM_PS4 == 0
    TCHAR Parm[4096] = TEXT("");

    const TCHAR* Cmd = FCommandLine::Get();

    // Catch the case where we want to override the map name on startup (used for connecting to other MP instances)
    if (FParse::Token(Cmd, Parm, ARRAY_COUNT(Parm), 0) && Parm[0] != '-')
    {
        // if we're 'overriding' with the default map anyway, don't set a bogus 'playing' state.
        if (!MainMenuMap.Contains(Parm))
        {
            FURL DefaultURL;
            DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);

            FURL URL(&DefaultURL, Parm, TRAVEL_Partial);

            if (URL.Valid)
            {
                UEngine* const Engine = GetEngine();

                FString Error;

                const EBrowseReturnVal::Type BrowseRet = Engine->Browse(*WorldContext, URL, Error);

                if (BrowseRet == EBrowseReturnVal::Success)
                {
                    // Success, we loaded the map, go directly to playing state
                    GotoState(MahjongGameInstanceState::Playing);
                    return;
                }
                else if (BrowseRet == EBrowseReturnVal::Pending)
                {
                    // Assume network connection
                    LoadFrontEndMap(MainMenuMap);
                    AddNetworkFailureHandlers();
                    ShowLoadingScreen();
                    GotoState(MahjongGameInstanceState::Playing);
                    return;
                }
            }
        }
    }
#endif

    GotoInitialState();
}

FName UMahjongGameInstance::GetInitialState()
{
#if MAHJONG_CONSOLE_UI	
    // Start in the welcome screen state on consoles
    return MahjongGameInstanceState::WelcomeScreen;
#else
    // On PC, go directly to the main menu
    return MahjongGameInstanceState::MainMenu;
#endif
}

void UMahjongGameInstance::GotoInitialState()
{
    GotoState(GetInitialState());
}

void UMahjongGameInstance::ShowMessageThenGotoState(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString, const FName& NewState, const bool OverrideExisting, TWeakObjectPtr< ULocalPlayer > PlayerOwner)
{
    UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Message: %s, NewState: %s"), *Message.ToString(), *NewState.ToString());

    const bool bAtWelcomeScreen = PendingState == MahjongGameInstanceState::WelcomeScreen || CurrentState == MahjongGameInstanceState::WelcomeScreen;

    // Never override the welcome screen
    if (bAtWelcomeScreen)
    {
        UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Ignoring due to higher message priority in queue (at welcome screen)."));
        return;
    }

    const bool bAlreadyAtMessageMenu = PendingState == MahjongGameInstanceState::MessageMenu || CurrentState == MahjongGameInstanceState::MessageMenu;
    const bool bAlreadyAtDestState = PendingState == NewState || CurrentState == NewState;

    // If we are already going to the message menu, don't override unless asked to
    if (bAlreadyAtMessageMenu && PendingMessage.NextState == NewState && !OverrideExisting)
    {
        UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Ignoring due to higher message priority in queue (check 1)."));
        return;
    }

    // If we are already going to the message menu, and the next dest is welcome screen, don't override
    if (bAlreadyAtMessageMenu && PendingMessage.NextState == MahjongGameInstanceState::WelcomeScreen)
    {
        UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Ignoring due to higher message priority in queue (check 2)."));
        return;
    }

    // If we are already at the dest state, don't override unless asked
    if (bAlreadyAtDestState && !OverrideExisting)
    {
        UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Ignoring due to higher message priority in queue (check 3)"));
        return;
    }

    PendingMessage.DisplayString = Message;
    PendingMessage.OKButtonString = OKButtonString;
    PendingMessage.CancelButtonString = CancelButtonString;
    PendingMessage.NextState = NewState;
    PendingMessage.PlayerOwner = PlayerOwner;

    if (CurrentState == MahjongGameInstanceState::MessageMenu)
    {
        UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Forcing new message"));
        EndMessageMenuState();
        BeginMessageMenuState();
    }
    else
    {
        GotoState(MahjongGameInstanceState::MessageMenu);
    }
}

void UMahjongGameInstance::ShowLoadingScreen()
{
    //@TODO
    //  This can be confusing, so here is what is happening:
    //	For LoadMap, we use the IMahjongGameLoadingScreenModule interface to show the load screen
    //  This is necessary since this is a blocking call, and our viewport loading screen won't get updated.
    //  We can't use IMahjongGameLoadingScreenModule for seamless travel though
    //  In this case, we just add a widget to the viewport, and have it update on the main thread
    //  To simplify things, we just do both, and you can't tell, one will cover the other if they both show at the same time
    /*IMahjongGameLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<IMahjongGameLoadingScreenModule>("MahjongGameLoadingScreen");
    if (LoadingScreenModule != nullptr)
    {
        LoadingScreenModule->StartInGameLoadingScreen();
    }

    UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());

    if (MahjongViewport != nullptr)
    {
        MahjongViewport->ShowLoadingScreen();
    }*/
}

bool UMahjongGameInstance::LoadFrontEndMap(const FString& MapName)
{
    bool bSuccess = true;

    // if already loaded, do nothing
    UWorld* const World = GetWorld();
    if (World)
    {
        FString const CurrentMapName = *World->PersistentLevel->GetOutermost()->GetName();
        if (CurrentMapName == MapName)
        {
            return bSuccess;
        }
    }

    FString Error;
    EBrowseReturnVal::Type BrowseRet = EBrowseReturnVal::Failure;
    FURL URL(
        *FString::Printf(TEXT("%s"), *MapName)
    );

    if (URL.Valid && !HasAnyFlags(RF_ClassDefaultObject)) //CastChecked<UEngine>() will fail if using Default__MahjongGameInstance, so make sure that we're not default
    {
        BrowseRet = GetEngine()->Browse(*WorldContext, URL, Error);

        // Handle failure.
        if (BrowseRet != EBrowseReturnVal::Success)
        {
            UE_LOG(LogLoad, Fatal, TEXT("%s"), *FString::Printf(TEXT("Failed to enter %s: %s. Please check the log for errors."), *MapName, *Error));
            bSuccess = false;
        }
    }
    return bSuccess;
}

AMahjongGameSession* UMahjongGameInstance::GetGameSession() const
{
    UWorld* const World = GetWorld();
    if (World)
    {
        AGameMode* const Game = World->GetAuthGameMode();
        if (Game)
        {
            return Cast<AMahjongGameSession>(Game->GameSession);
        }
    }

    return nullptr;
}

void UMahjongGameInstance::TravelLocalSessionFailure(UWorld *World, ETravelFailure::Type FailureType, const FString& ReasonString)
{
    AMahjongPlayerController_Menu* FirstPC = Cast<AMahjongPlayerController_Menu>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (FirstPC)
    {
        FText ReturnReason = NSLOCTEXT("NetworkErrors", "JoinSessionFailed", "Join Session failed.");
        if (ReasonString.IsEmpty() == false)
        {
            ReturnReason = FText::Format(NSLOCTEXT("NetworkErrors", "JoinSessionFailedReasonFmt", "Join Session failed. {0}"), FText::FromString(ReasonString));
        }

        FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
        ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
    }
}

void UMahjongGameInstance::ShowMessageThenGoMain(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString)
{
    ShowMessageThenGotoState(Message, OKButtonString, CancelButtonString, MahjongGameInstanceState::MainMenu);
}

void UMahjongGameInstance::GotoState(FName NewState)
{
    UE_LOG(LogOnline, Log, TEXT("GotoState: NewState: %s"), *NewState.ToString());

    PendingState = NewState;
}

void UMahjongGameInstance::MaybeChangeState()
{
    if ((PendingState != CurrentState) && (PendingState != MahjongGameInstanceState::None))
    {
        FName const OldState = CurrentState;

        // end current state
        EndCurrentState(PendingState);

        // begin new state
        BeginNewState(PendingState, OldState);

        // clear pending change
        PendingState = MahjongGameInstanceState::None;
    }
}

void UMahjongGameInstance::EndCurrentState(FName NextState)
{
    // per-state custom ending code here
    if (CurrentState == MahjongGameInstanceState::PendingInvite)
    {
        EndPendingInviteState();
    }
    else if (CurrentState == MahjongGameInstanceState::WelcomeScreen)
    {
        EndWelcomeScreenState();
    }
    else if (CurrentState == MahjongGameInstanceState::MainMenu)
    {
        EndMainMenuState();
    }
    else if (CurrentState == MahjongGameInstanceState::MessageMenu)
    {
        EndMessageMenuState();
    }
    else if (CurrentState == MahjongGameInstanceState::Playing)
    {
        EndPlayingState();
    }

    CurrentState = MahjongGameInstanceState::None;
}

void UMahjongGameInstance::BeginNewState(FName NewState, FName PrevState)
{
    // per-state custom starting code here

    if (NewState == MahjongGameInstanceState::PendingInvite)
    {
        BeginPendingInviteState();
    }
    else if (NewState == MahjongGameInstanceState::WelcomeScreen)
    {
        BeginWelcomeScreenState();
    }
    else if (NewState == MahjongGameInstanceState::MainMenu)
    {
        BeginMainMenuState();
    }
    else if (NewState == MahjongGameInstanceState::MessageMenu)
    {
        BeginMessageMenuState();
    }
    else if (NewState == MahjongGameInstanceState::Playing)
    {
        BeginPlayingState();
    }

    CurrentState = NewState;
}

void UMahjongGameInstance::BeginPendingInviteState()
{
    if (LoadFrontEndMap(MainMenuMap))
    {
        StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UMahjongGameInstance::OnUserCanPlayInvite), EUserPrivileges::CanPlayOnline, PendingInvite.UserId);
    }
    else
    {
        GotoState(MahjongGameInstanceState::WelcomeScreen);
    }
}

void UMahjongGameInstance::EndPendingInviteState()
{
    // cleanup in case the state changed before the pending invite was handled.
    CleanupOnlinePrivilegeTask();
}

void UMahjongGameInstance::BeginWelcomeScreenState()
{
    //this must come before split screen player removal so that the OSS sets all players to not using online features.
    SetIsOnline(false);

    // Remove any possible splitscren players
    while (LocalPlayers.Num() > 1)
    {
        ULocalPlayer* const PlayerToRemove = LocalPlayers.Last();
        RemoveExistingLocalPlayer(PlayerToRemove);
    }

    LoadFrontEndMap(WelcomeScreenMap);

    ULocalPlayer* const LocalPlayer = GetFirstGamePlayer();
    LocalPlayer->SetCachedUniqueNetId(nullptr);
    //@TODO
    //check(!WelcomeMenuUI.IsValid());
    //WelcomeMenuUI = MakeShareable(new FMahjongWelcomeMenu);
    //WelcomeMenuUI->Construct(this);
    //WelcomeMenuUI->AddToGameViewport();

    // Disallow splitscreen (we will allow while in the playing state)
    GetGameViewportClient()->SetDisableSplitscreenOverride(true);
}

void UMahjongGameInstance::EndWelcomeScreenState()
{
    //@TODO
    /*if (WelcomeMenuUI.IsValid())
    {
        WelcomeMenuUI->RemoveFromGameViewport();
        WelcomeMenuUI = nullptr;
    }*/
}

void UMahjongGameInstance::SetPresenceForLocalPlayers(const FVariantData& PresenceData)
{
    IOnlinePresencePtr Presence = Online::GetPresenceInterface();
    if (Presence.IsValid())
    {
        for (int i = 0; i < LocalPlayers.Num(); ++i)
        {
            const TSharedPtr<const FUniqueNetId> UserId = LocalPlayers[i]->GetPreferredUniqueNetId();

            if (UserId.IsValid())
            {
                FOnlineUserPresenceStatus PresenceStatus;
                PresenceStatus.Properties.Add(DefaultPresenceKey, PresenceData);

                Presence->SetPresence(*UserId, PresenceStatus);
            }
        }
    }
}

void UMahjongGameInstance::BeginMainMenuState()
{
    // Make sure we're not showing the loadscreen
    //@TODO
    /*UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());

    if (MahjongViewport)
    {
        MahjongViewport->HideLoadingScreen();
    }*/

    SetIsOnline(false);

    // Disallow splitscreen
    GetGameViewportClient()->SetDisableSplitscreenOverride(true);

    // Remove any possible splitscren players
    while (LocalPlayers.Num() > 1)
    {
        ULocalPlayer* const PlayerToRemove = LocalPlayers.Last();
        RemoveExistingLocalPlayer(PlayerToRemove);
    }

    // Set presence to menu state for the owning player
    SetPresenceForLocalPlayers(FVariantData(FString(TEXT("OnMenu"))));

    // load startup map
    LoadFrontEndMap(MainMenuMap);

    // player 0 gets to own the UI
    ULocalPlayer* Player = GetFirstGamePlayer();

    //@TODO
    /*MainMenuUI = MakeShareable(new FMahjongMainMenu());
    MainMenuUI->Construct(this, Player);
    MainMenuUI->AddMenuToGameViewport();*/

#if !MAHJONG_CONSOLE_UI
    // The cached unique net ID is usually set on the welcome screen, but there isn't
    // one on PC/Mac, so do it here.
    if (Player)
    {
        Player->SetControllerId(0);
        Player->SetCachedUniqueNetId(Player->GetUniqueNetIdFromCachedControllerId());
    }
#endif

    RemoveNetworkFailureHandlers();
}

void UMahjongGameInstance::EndMainMenuState()
{
    //@TODO
    /*if (MainMenuUI.IsValid())
    {
        MainMenuUI->RemoveMenuFromGameViewport();
        MainMenuUI = nullptr;
    }*/
}

void UMahjongGameInstance::BeginMessageMenuState()
{
    if (PendingMessage.DisplayString.IsEmpty())
    {
        UE_LOG(LogOnlineGame, Warning, TEXT("UMahjongGameInstance::BeginMessageMenuState: Display string is empty"));
        GotoInitialState();
        return;
    }

    // Make sure we're not showing the loadscreen
    //@TODO
    /*UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());

    if (MahjongViewport != nullptr)
    {
        MahjongViewport->HideLoadingScreen();
    }

    check(!MessageMenuUI.IsValid());
    MessageMenuUI = MakeShareable(new FMahjongMessageMenu);
    MessageMenuUI->Construct(this, PendingMessage.PlayerOwner, PendingMessage.DisplayString, PendingMessage.OKButtonString, PendingMessage.CancelButtonString, PendingMessage.NextState);
    */
    PendingMessage.DisplayString = FText::GetEmpty();
}

void UMahjongGameInstance::EndMessageMenuState()
{
    //@TODO
    /*if (MessageMenuUI.IsValid())
    {
        MessageMenuUI->RemoveFromGameViewport();
        MessageMenuUI = nullptr;
    }*/
}

void UMahjongGameInstance::BeginPlayingState()
{
    //bPendingEnableSplitscreen = true;

    // Set presence for playing in a map
    SetPresenceForLocalPlayers(FVariantData(FString(TEXT("InGame"))));

    // Make sure viewport has focus
    FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void UMahjongGameInstance::EndPlayingState()
{
    // Disallow splitscreen
    GetGameViewportClient()->SetDisableSplitscreenOverride(true);

    // Clear the players' presence information
    SetPresenceForLocalPlayers(FVariantData(FString(TEXT("OnMenu"))));

    UWorld* World = GetWorld();
    AMahjongGameState* GameState = World ? World->GetGameState<AMahjongGameState>() : nullptr;

    if (GameState)
    {
        // Send round end events for local players
        for (int i = 0; i < LocalPlayers.Num(); ++i)
        {
            AMahjongPlayerController* MahjongPC = Cast<AMahjongPlayerController>(LocalPlayers[i]->PlayerController);
            if (MahjongPC)
            {
                // Assuming you can't win if you quit early
                MahjongPC->ClientSendRoundEndEvent(false, GameState->ElapsedTime);
            }
        }

        // Give the game state a chance to cleanup first
        GameState->RequestFinishAndExitToMainMenu();
    }
    else
    {
        // If there is no game state, make sure the session is in a good state
        CleanupSessionOnReturnToMenu();
    }
}

void UMahjongGameInstance::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogOnline, Log, TEXT("UMahjongGameInstance::OnEndSessionComplete: Session=%s bWasSuccessful=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid())
        {
            Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
            Sessions->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);
            Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
        }
    }

    // continue
    CleanupSessionOnReturnToMenu();
}

void UMahjongGameInstance::CleanupSessionOnReturnToMenu()
{
    bool bPendingOnlineOp = false;

    // end online game and then destroy it
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    IOnlineSessionPtr Sessions = OnlineSubsystem ? OnlineSubsystem->GetSessionInterface() : nullptr;

    if (Sessions.IsValid())
    {
        EOnlineSessionState::Type SessionState = Sessions->GetSessionState(GameSessionName);
        UE_LOG(LogOnline, Log, TEXT("Session %s is '%s'"), *GameSessionName.ToString(), EOnlineSessionState::ToString(SessionState));

        if (EOnlineSessionState::InProgress == SessionState)
        {
            UE_LOG(LogOnline, Log, TEXT("Ending session %s on return to main menu"), *GameSessionName.ToString());
            OnEndSessionCompleteDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
            Sessions->EndSession(GameSessionName);
            bPendingOnlineOp = true;
        }
        else if (EOnlineSessionState::Ending == SessionState)
        {
            UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to end on return to main menu"), *GameSessionName.ToString());
            OnEndSessionCompleteDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
            bPendingOnlineOp = true;
        }
        else if (EOnlineSessionState::Ended == SessionState || EOnlineSessionState::Pending == SessionState)
        {
            UE_LOG(LogOnline, Log, TEXT("Destroying session %s on return to main menu"), *GameSessionName.ToString());
            OnDestroySessionCompleteDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
            Sessions->DestroySession(GameSessionName);
            bPendingOnlineOp = true;
        }
        else if (EOnlineSessionState::Starting == SessionState)
        {
            UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to start, and then we will end it to return to main menu"), *GameSessionName.ToString());
            OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
            bPendingOnlineOp = true;
        }
    }

    if (!bPendingOnlineOp)
    {
        GEngine->HandleDisconnect(GetWorld(), GetWorld()->GetNetDriver());
    }
}

void UMahjongGameInstance::LabelPlayerAsQuitter(ULocalPlayer* LocalPlayer) const
{
    AMahjongPlayerState* PlayerState = LocalPlayer && LocalPlayer->PlayerController ? Cast<AMahjongPlayerState>(LocalPlayer->PlayerController->PlayerState) : nullptr;
    if (PlayerState)
    {
        PlayerState->SetQuitter(true);
    }
}

void UMahjongGameInstance::RemoveNetworkFailureHandlers()
{
    // Remove the local session/travel failure bindings if they exist
    if (GEngine->OnTravelFailure().IsBoundToObject(this))
    {
        GEngine->OnTravelFailure().Remove(TravelLocalSessionFailureDelegateHandle);
    }
}

void UMahjongGameInstance::AddNetworkFailureHandlers()
{
    // Add network/travel error handlers (if they are not already there)
    if (!GEngine->OnTravelFailure().IsBoundToObject(this))
    {
        TravelLocalSessionFailureDelegateHandle = GEngine->OnTravelFailure().AddUObject(this, &UMahjongGameInstance::TravelLocalSessionFailure);
    }
}

bool UMahjongGameInstance::HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL)
{
    if (!GetIsOnline())
    {
        // Offline game, just go straight to map
        ShowLoadingScreen();
        GotoState(MahjongGameInstanceState::Playing);

        // Travel to the specified match URL
        TravelURL = InTravelURL;
        GetWorld()->ServerTravel(TravelURL);
        return true;
    }

    // Online game
    AMahjongGameSession* GameSession = GetGameSession();
    if (GameSession)
    {
        // add callback delegate for completion
        OnCreatePresenceSessionCompleteDelegateHandle = GameSession->OnCreatePresenceSessionComplete().AddUObject(this, &UMahjongGameInstance::OnCreatePresenceSessionComplete);

        TravelURL = InTravelURL;
        bool const bIsLanMatch = InTravelURL.Contains(TEXT("?bIsLanMatch"));

        //determine the map name from the travelURL
        const FString& MapNameSubStr = "/Game/Maps/";
        const FString& ChoppedMapName = TravelURL.RightChop(MapNameSubStr.Len());
        const FString& MapName = ChoppedMapName.LeftChop(ChoppedMapName.Len() - ChoppedMapName.Find("?game"));

        if (GameSession->HostSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, GameType, MapName, bIsLanMatch, true, GameSession->GetMaxPlayers()))
        {
            // If any error occured in the above, pending state would be set
            if ((PendingState == CurrentState) || (PendingState == MahjongGameInstanceState::None))
            {
                // Go ahead and go into loading state now
                // If we fail, the delegate will handle showing the proper messaging and move to the correct state
                ShowLoadingScreen();
                GotoState(MahjongGameInstanceState::Playing);
                return true;
            }
        }
    }

    return false;
}

bool UMahjongGameInstance::JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults)
{
    // needs to tear anything down based on current state?

    AMahjongGameSession* GameSession = GetGameSession();
    if (GameSession)
    {
        AddNetworkFailureHandlers();

        OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSessionComplete().AddUObject(this, &UMahjongGameInstance::OnJoinSessionComplete);
        if (GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, SessionIndexInSearchResults))
        {
            // If any error occured in the above, pending state would be set
            if ((PendingState == CurrentState) || (PendingState == MahjongGameInstanceState::None))
            {
                // Go ahead and go into loading state now
                // If we fail, the delegate will handle showing the proper messaging and move to the correct state
                ShowLoadingScreen();
                GotoState(MahjongGameInstanceState::Playing);
                return true;
            }
        }
    }

    return false;
}

bool UMahjongGameInstance::JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult)
{
    // needs to tear anything down based on current state?
    AMahjongGameSession* GameSession = GetGameSession();
    if (GameSession)
    {
        AddNetworkFailureHandlers();

        OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSessionComplete().AddUObject(this, &UMahjongGameInstance::OnJoinSessionComplete);
        if (GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, SearchResult))
        {
            // If any error occured in the above, pending state would be set
            if ((PendingState == CurrentState) || (PendingState == MahjongGameInstanceState::None))
            {
                // Go ahead and go into loading state now
                // If we fail, the delegate will handle showing the proper messaging and move to the correct state
                ShowLoadingScreen();
                GotoState(MahjongGameInstanceState::Playing);
                return true;
            }
        }
    }

    return false;
}

void UMahjongGameInstance::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
    // unhook the delegate
    AMahjongGameSession* GameSession = GetGameSession();
    if (GameSession)
    {
        GameSession->OnJoinSessionComplete().Remove(OnJoinSessionCompleteDelegateHandle);
    }

    // Add the splitscreen player if one exists
    if (Result == EOnJoinSessionCompleteResult::Success && LocalPlayers.Num() > 1)
    {
        IOnlineSessionPtr Sessions = Online::GetSessionInterface();
        if (Sessions.IsValid() && LocalPlayers[1]->GetPreferredUniqueNetId().IsValid())
        {
            Sessions->RegisterLocalPlayer(*LocalPlayers[1]->GetPreferredUniqueNetId(), GameSessionName,
                FOnRegisterLocalPlayerCompleteDelegate::CreateUObject(this, &UMahjongGameInstance::OnRegisterJoiningLocalPlayerComplete));
        }
    }
    else
    {
        // We either failed or there is only a single local user
        FinishJoinSession(Result);
    }
}

void UMahjongGameInstance::FinishJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        FText ReturnReason;
        switch (Result)
        {
            case EOnJoinSessionCompleteResult::SessionIsFull:
                ReturnReason = NSLOCTEXT("NetworkErrors", "JoinSessionFailed", "Game is full.");
                break;
            case EOnJoinSessionCompleteResult::SessionDoesNotExist:
                ReturnReason = NSLOCTEXT("NetworkErrors", "JoinSessionFailed", "Game no longer exists.");
                break;
            default:
                ReturnReason = NSLOCTEXT("NetworkErrors", "JoinSessionFailed", "Join failed.");
                break;
        }

        FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
        RemoveNetworkFailureHandlers();
        ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
        return;
    }

    InternalTravelToSession(GameSessionName);
}

void UMahjongGameInstance::OnRegisterJoiningLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result)
{
    FinishJoinSession(Result);
}

void UMahjongGameInstance::InternalTravelToSession(const FName& SessionName)
{
    APlayerController* PlayerController = GetFirstLocalPlayerController();

    if (!PlayerController)
    {
        FText ReturnReason = NSLOCTEXT("NetworkErrors", "InvalidPlayerController", "Invalid Player Controller");
        FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
        RemoveNetworkFailureHandlers();
        ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
        return;
    }

    // travel to session
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

    if (!OnlineSubsystem)
    {
        FText ReturnReason = NSLOCTEXT("NetworkErrors", "OSSMissing", "OSS missing");
        FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
        RemoveNetworkFailureHandlers();
        ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
        return;
    }

    FString URL;
    IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();

    if (!Sessions.IsValid() || !Sessions->GetResolvedConnectString(SessionName, URL))
    {
        FText FailReason = NSLOCTEXT("NetworkErrors", "TravelSessionFailed", "Travel to Session failed.");
        FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
        ShowMessageThenGoMain(FailReason, OKButton, FText::GetEmpty());
        UE_LOG(LogOnlineGame, Warning, TEXT("Failed to travel to session upon joining it"));
        return;
    }

    PlayerController->ClientTravel(URL, TRAVEL_Absolute);
}

void UMahjongGameInstance::OnCreatePresenceSessionComplete(FName SessionName, bool bWasSuccessful)
{
    AMahjongGameSession* GameSession = GetGameSession();
    if (GameSession)
    {
        GameSession->OnCreatePresenceSessionComplete().Remove(OnCreatePresenceSessionCompleteDelegateHandle);

        // Add the splitscreen player if one exists
        if (bWasSuccessful && LocalPlayers.Num() > 1)
        {
            IOnlineSessionPtr Sessions = Online::GetSessionInterface();
            if (Sessions.IsValid() && LocalPlayers[1]->GetPreferredUniqueNetId().IsValid())
            {
                Sessions->RegisterLocalPlayer(*LocalPlayers[1]->GetPreferredUniqueNetId(), GameSessionName,
                    FOnRegisterLocalPlayerCompleteDelegate::CreateUObject(this, &UMahjongGameInstance::OnRegisterLocalPlayerComplete));
            }
        }
        else
        {
            // We either failed or there is only a single local user
            FinishSessionCreation(bWasSuccessful ? EOnJoinSessionCompleteResult::Success : EOnJoinSessionCompleteResult::UnknownError);
        }
    }
}

bool UMahjongGameInstance::FindSessions(ULocalPlayer* PlayerOwner, bool bFindLAN)
{
    bool bResult = false;

    check(PlayerOwner != nullptr);
    if (PlayerOwner)
    {
        AMahjongGameSession* GameSession = GetGameSession();
        if (GameSession)
        {
            GameSession->OnFindSessionsComplete().RemoveAll(this);
            OnSearchSessionsCompleteDelegateHandle = GameSession->OnFindSessionsComplete().AddUObject(this, &UMahjongGameInstance::OnSearchSessionsComplete);

            GameSession->FindSessions(PlayerOwner->GetPreferredUniqueNetId(), GameSessionName, bFindLAN, true);

            bResult = true;
        }
    }

    return bResult;
}

void UMahjongGameInstance::OnSearchSessionsComplete(bool bWasSuccessful)
{
    AMahjongGameSession* Session = GetGameSession();
    if (Session)
    {
        Session->OnFindSessionsComplete().Remove(OnSearchSessionsCompleteDelegateHandle);
    }
}

bool UMahjongGameInstance::Tick(float DeltaSeconds)
{
    // Dedicated server doesn't need to worry about game state
    if (IsRunningDedicatedServer() == true)
    {
        return true;
    }

    MaybeChangeState();

    //@TODO
    //UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());

    if (CurrentState != MahjongGameInstanceState::WelcomeScreen)
    {
        // If at any point we aren't licensed (but we are after welcome screen) bounce them back to the welcome screen
        //@TODO
        //if (MAHJONG_REQUIRE_OSS && !bIsLicensed && CurrentState != MahjongGameInstanceState::None && !MahjongViewport->IsShowingDialog())
        if (false)
        {
            const FText ReturnReason = NSLOCTEXT("ProfileMessages", "NeedLicense", "The signed in users do not have a license for this game. Please purchase this game from the platform Marketplace or sign in a user with a valid license.");
            const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

            ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), MahjongGameInstanceState::WelcomeScreen);
        }

        // Show controller disconnected dialog if any local players have an invalid controller
        //@TODO
        //if (MahjongViewport && !MahjongViewport->IsShowingDialog())
        if (false)
        {
            for (int i = 0; i < LocalPlayers.Num(); ++i)
            {
                if (LocalPlayers[i] && LocalPlayers[i]->GetControllerId() == -1)
                {
                    //@TODO
                    /*MahjongViewport->ShowDialog(
                        LocalPlayers[i],
                        EMahjongDialogType::ControllerDisconnected,
                        FText::Format(NSLOCTEXT("ProfileMessages", "PlayerReconnectControllerFmt", "Player {0}, please reconnect your controller."), FText::AsNumber(i + 1)),
#ifdef PLATFORM_PS4
                        NSLOCTEXT("DialogButtons", "PS4_CrossButtonContinue", "Cross Button - Continue"),
#else
                        NSLOCTEXT("DialogButtons", "AButtonContinue", "A - Continue"),
#endif
                        FText::GetEmpty(),
                        FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnControllerReconnectConfirm),
                        FOnClicked()
                    );*/
                }
            }
        }
    }

    // If we have a pending invite, and we are at the welcome screen, and the session is properly shut down, accept it
    if (PendingInvite.UserId.IsValid() && PendingInvite.bPrivilegesCheckedAndAllowed && CurrentState == MahjongGameInstanceState::PendingInvite)
    {
        IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
        IOnlineSessionPtr Sessions = OnlineSubsystem ? OnlineSubsystem->GetSessionInterface() : nullptr;

        if (Sessions.IsValid())
        {
            EOnlineSessionState::Type SessionState = Sessions->GetSessionState(GameSessionName);

            if (SessionState == EOnlineSessionState::NoSession)
            {
                ULocalPlayer* NewPlayerOwner = GetFirstGamePlayer();

                if (NewPlayerOwner != nullptr)
                {
                    NewPlayerOwner->SetControllerId(PendingInvite.ControllerId);
                    NewPlayerOwner->SetCachedUniqueNetId(PendingInvite.UserId);
                    SetIsOnline(true);
                    JoinSession(NewPlayerOwner, PendingInvite.InviteResult);
                }

                PendingInvite.UserId.Reset();
            }
        }
    }

    return true;
}

bool UMahjongGameInstance::HandleOpenCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* InWorld)
{
    bool const bOpenSuccessful = Super::HandleOpenCommand(Cmd, Ar, InWorld);
    if (bOpenSuccessful)
    {
        GotoState(MahjongGameInstanceState::Playing);
    }

    return bOpenSuccessful;
}

void UMahjongGameInstance::HandleSignInChangeMessaging()
{
    // Master user signed out, go to initial state (if we aren't there already)
    if (CurrentState != GetInitialState())
    {
#if MAHJONG_CONSOLE_UI
        // Display message on consoles
        const FText ReturnReason = NSLOCTEXT("ProfileMessages", "SignInChange", "Sign in status change occurred.");
        const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

        ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), GetInitialState());
#else								
        GotoInitialState();
#endif
    }
}

void UMahjongGameInstance::HandleUserLoginChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId)
{
    const bool bDowngraded = (LoginStatus == ELoginStatus::NotLoggedIn && !GetIsOnline()) || (LoginStatus != ELoginStatus::LoggedIn && GetIsOnline());

    UE_LOG(LogOnline, Log, TEXT("HandleUserLoginChanged: bDownGraded: %i"), (int)bDowngraded);

    TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
    bIsLicensed = GenericApplication->ApplicationLicenseValid();

    // Find the local player associated with this unique net id
    ULocalPlayer* LocalPlayer = FindLocalPlayerFromUniqueNetId(UserId);

    // If this user is signed out, but was previously signed in, punt to welcome (or remove splitscreen if that makes sense)
    if (LocalPlayer)
    {
        if (MAHJONG_REQUIRE_OSS && bDowngraded)
        {
            UE_LOG(LogOnline, Log, TEXT("HandleUserLoginChanged: Player logged out: %s"), *UserId.ToString());

            LabelPlayerAsQuitter(LocalPlayer);

            // Check to see if this was the master, or if this was a split-screen player on the client
            if (LocalPlayer == GetFirstGamePlayer() || GetIsOnline())
            {
                HandleSignInChangeMessaging();
            }
            else
            {
                // Remove local split-screen players from the list
                RemoveExistingLocalPlayer(LocalPlayer);
            }
        }
    }
}

void UMahjongGameInstance::HandleAppWillDeactivate()
{
    if (CurrentState == MahjongGameInstanceState::Playing)
    {
        // Just have the first player controller pause the game.
        UWorld* GameWorld = GetWorld();
        if (GameWorld)
        {
            // protect against a second pause menu loading on top of an existing one if someone presses the Jewel / PS buttons.
            bool bNeedsPause = true;
            for (FConstControllerIterator It = GameWorld->GetControllerIterator(); It; ++It)
            {
                AMahjongPlayerController* Controller = Cast<AMahjongPlayerController>(*It);
                if (Controller && (Controller->IsPaused() || Controller->IsGameMenuVisible()))
                {
                    bNeedsPause = false;
                    break;
                }
            }

            if (bNeedsPause)
            {
                AMahjongPlayerController* const Controller = Cast<AMahjongPlayerController>(GameWorld->GetFirstPlayerController());
                if (Controller)
                {
                    //@TODO
                    //Controller->ShowInGameMenu();
                }
            }
        }
    }
}

void UMahjongGameInstance::HandleAppSuspend()
{
    // Players will lose connection on resume. However it is possible the game will exit before we get a resume, so we must kick off round end events here.
    UE_LOG(LogOnline, Warning, TEXT("UMahjongGameInstance::HandleAppSuspend"));
    UWorld* World = GetWorld();
    AMahjongGameState* GameState = World != nullptr ? World->GetGameState<AMahjongGameState>() : nullptr;

    if (CurrentState != MahjongGameInstanceState::None && CurrentState != GetInitialState())
    {
        UE_LOG(LogOnline, Warning, TEXT("UMahjongGameInstance::HandleAppSuspend: Sending round end event for players"));

        // Send round end events for local players
        for (int i = 0; i < LocalPlayers.Num(); ++i)
        {
            AMahjongPlayerController* MahjongPC = Cast<AMahjongPlayerController>(LocalPlayers[i]->PlayerController);
            if (MahjongPC)
            {
                // Assuming you can't win if you quit early
                MahjongPC->ClientSendRoundEndEvent(false, GameState->ElapsedTime);
            }
        }
    }
}

void UMahjongGameInstance::HandleAppResume()
{
    UE_LOG(LogOnline, Log, TEXT("UMahjongGameInstance::HandleAppResume"));

    if (CurrentState != MahjongGameInstanceState::None && CurrentState != GetInitialState())
    {
        UE_LOG(LogOnline, Warning, TEXT("UMahjongGameInstance::HandleAppResume: Attempting to sign out players"));

        for (int32 i = 0; i < LocalPlayers.Num(); ++i)
        {
            if (LocalPlayers[i]->GetCachedUniqueNetId().IsValid() && !IsLocalPlayerOnline(LocalPlayers[i]))
            {
                UE_LOG(LogOnline, Log, TEXT("UMahjongGameInstance::HandleAppResume: Signed out during resume."));
                HandleSignInChangeMessaging();
                break;
            }
        }
    }
}

void UMahjongGameInstance::HandleAppLicenseUpdate()
{
    TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
    bIsLicensed = GenericApplication->ApplicationLicenseValid();
}

void UMahjongGameInstance::HandleSafeFrameChanged()
{
    UCanvas::UpdateAllCanvasSafeZoneData();
}

void UMahjongGameInstance::RemoveExistingLocalPlayer(ULocalPlayer* ExistingPlayer)
{
    check(ExistingPlayer);
    if (ExistingPlayer->PlayerController != nullptr)
    {
        // Kill the player
        AMahjongCharacter* MyPawn = Cast<AMahjongCharacter>(ExistingPlayer->PlayerController->GetPawn());
        if (MyPawn)
        {
            MyPawn->KilledBy(nullptr);
        }
    }

    // Remove local split-screen players from the list
    RemoveLocalPlayer(ExistingPlayer);
}

FReply UMahjongGameInstance::OnPairingUsePreviousProfile()
{
    // Do nothing (except hide the message) if they want to continue using previous profile
    //@TODO
    /*UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());

    if (MahjongViewport != nullptr)
    {
        MahjongViewport->HideDialog();
    }*/

    return FReply::Handled();
}

FReply UMahjongGameInstance::OnPairingUseNewProfile()
{
    HandleSignInChangeMessaging();
    return FReply::Handled();
}

void UMahjongGameInstance::HandleControllerPairingChanged(int GameUserIndex, const FUniqueNetId& PreviousUser, const FUniqueNetId& NewUser)
{
    UE_LOG(LogOnlineGame, Log, TEXT("UMahjongGameInstance::HandleControllerPairingChanged GameUserIndex %d PreviousUser '%s' NewUser '%s'"),
        GameUserIndex, *PreviousUser.ToString(), *NewUser.ToString());

    if (CurrentState == MahjongGameInstanceState::WelcomeScreen)
    {
        // Don't care about pairing changes at welcome screen
        return;
    }

#if MAHJONG_CONSOLE_UI && PLATFORM_XBOXONE
    if (IgnorePairingChangeForControllerId != -1 && GameUserIndex == IgnorePairingChangeForControllerId)
    {
        // We were told to ignore
        IgnorePairingChangeForControllerId = -1;	// Reset now so there there is no chance this remains in a bad state
        return;
    }

    if (PreviousUser.IsValid() && !NewUser.IsValid())
    {
        // Treat this as a disconnect or signout, which is handled somewhere else
        return;
    }

    if (!PreviousUser.IsValid() && NewUser.IsValid())
    {
        // Treat this as a signin
        ULocalPlayer* ControlledLocalPlayer = FindLocalPlayerFromControllerId(GameUserIndex);

        if (ControlledLocalPlayer != nullptr && !ControlledLocalPlayer->GetCachedUniqueNetId().IsValid())
        {
            // If a player that previously selected "continue without saving" signs into this controller, move them back to welcome screen
            HandleSignInChangeMessaging();
        }

        return;
    }

    // Find the local player currently being controlled by this controller
    ULocalPlayer* ControlledLocalPlayer = FindLocalPlayerFromControllerId(GameUserIndex);

    // See if the newly assigned profile is in our local player list
    ULocalPlayer* NewLocalPlayer = FindLocalPlayerFromUniqueNetId(NewUser);

    // If the local player being controlled is not the target of the pairing change, then give them a chance 
    // to continue controlling the old player with this controller
    if (ControlledLocalPlayer != nullptr && ControlledLocalPlayer != NewLocalPlayer)
    {
        UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());

        if (MahjongViewport != nullptr)
        {
            MahjongViewport->ShowDialog(
                nullptr,
                EMahjongDialogType::Generic,
                NSLOCTEXT("ProfileMessages", "PairingChanged", "Your controller has been paired to another profile, would you like to switch to this new profile now? Selecting YES will sign out of the previous profile."),
                NSLOCTEXT("DialogButtons", "YES", "A - YES"),
                NSLOCTEXT("DialogButtons", "NO", "B - NO"),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnPairingUseNewProfile),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnPairingUsePreviousProfile)
            );
        }
    }
#endif
}

void UMahjongGameInstance::HandleControllerConnectionChange(bool bIsConnection, int32 Unused, int32 GameUserIndex)
{
    UE_LOG(LogOnlineGame, Log, TEXT("UMahjongGameInstance::HandleControllerConnectionChange bIsConnection %d GameUserIndex %d"),
        bIsConnection, GameUserIndex);

    if (!bIsConnection)
    {
        // Controller was disconnected

        // Find the local player associated with this user index
        ULocalPlayer* LocalPlayer = FindLocalPlayerFromControllerId(GameUserIndex);

        if (LocalPlayer == nullptr)
        {
            return;		// We don't care about players we aren't tracking
        }

        // Invalidate this local player's controller id.
        LocalPlayer->SetControllerId(-1);
    }
}

FReply UMahjongGameInstance::OnControllerReconnectConfirm()
{
    //@TODO
    /*UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());
    if (MahjongViewport)
    {
        MahjongViewport->HideDialog();
    }*/

    return FReply::Handled();
}

TSharedPtr< const FUniqueNetId > UMahjongGameInstance::GetUniqueNetIdFromControllerId(const int ControllerId)
{
    IOnlineIdentityPtr OnlineIdentity = Online::GetIdentityInterface();

    if (OnlineIdentity.IsValid())
    {
        TSharedPtr<const FUniqueNetId> UniqueId = OnlineIdentity->GetUniquePlayerId(ControllerId);

        if (UniqueId.IsValid())
        {
            return UniqueId;
        }
    }

    return nullptr;
}

void UMahjongGameInstance::SetIsOnline(bool bInIsOnline)
{
    bIsOnline = bInIsOnline;
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

    if (OnlineSubsystem)
    {
        for (int32 i = 0; i < LocalPlayers.Num(); ++i)
        {
            ULocalPlayer* LocalPlayer = LocalPlayers[i];

            TSharedPtr<const FUniqueNetId> PlayerId = LocalPlayer->GetPreferredUniqueNetId();
            if (PlayerId.IsValid())
            {
                OnlineSubsystem->SetUsingMultiplayerFeatures(*PlayerId, bIsOnline);
            }
        }
    }
}

void UMahjongGameInstance::TravelToSession(const FName& SessionName)
{
    AddNetworkFailureHandlers();
    ShowLoadingScreen();
    GotoState(MahjongGameInstanceState::Playing);
    InternalTravelToSession(SessionName);
}

void UMahjongGameInstance::SetIgnorePairingChangeForControllerId(const int32 ControllerId)
{
    IgnorePairingChangeForControllerId = ControllerId;
}

bool UMahjongGameInstance::IsLocalPlayerOnline(ULocalPlayer* LocalPlayer)
{
    if (LocalPlayer == nullptr)
    {
        return false;
    }
    const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface();
        if (IdentityInterface.IsValid())
        {
            TSharedPtr<const FUniqueNetId> UniqueId = LocalPlayer->GetCachedUniqueNetId();
            if (UniqueId.IsValid())
            {
                ELoginStatus::Type LoginStatus = IdentityInterface->GetLoginStatus(*UniqueId);
                if (LoginStatus == ELoginStatus::LoggedIn)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool UMahjongGameInstance::ValidatePlayerForOnlinePlay(ULocalPlayer* LocalPlayer)
{
    // Get the viewport
    //@TODO
    //UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());

#if PLATFORM_XBOXONE
    if (CurrentConnectionStatus != EOnlineServerConnectionStatus::Connected)
    {
        // Don't let them play online if they aren't connected to Xbox LIVE
        if (MahjongViewport != nullptr)
        {
            const FText Msg = NSLOCTEXT("NetworkFailures", "ServiceDisconnected", "You must be connected to the Xbox LIVE service to play online.");
            const FText OKButtonString = NSLOCTEXT("DialogButtons", "OKAY", "OK");

            MahjongViewport->ShowDialog(
                nullptr,
                EMahjongDialogType::Generic,
                Msg,
                OKButtonString,
                FText::GetEmpty(),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric)
            );
        }

        return false;
    }
#endif

    if (!IsLocalPlayerOnline(LocalPlayer))
    {
        // Don't let them play online if they aren't online
        
        //@TODO
        //if (MahjongViewport)
        if (false)
        {
            const FText Msg = NSLOCTEXT("NetworkFailures", "MustBeSignedIn", "You must be signed in to play online");
            const FText OKButtonString = NSLOCTEXT("DialogButtons", "OKAY", "OK");

            /*MahjongViewport->ShowDialog(
                nullptr,
                EMahjongDialogType::Generic,
                Msg,
                OKButtonString,
                FText::GetEmpty(),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric)
            );*/
        }

        return false;
    }

    return true;
}

FReply UMahjongGameInstance::OnConfirmGeneric()
{
    //@TODO
    /*UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());
    if (MahjongViewport)
    {
        MahjongViewport->HideDialog();
    }*/

    return FReply::Handled();
}

void UMahjongGameInstance::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate, EUserPrivileges::Type Privilege, TSharedPtr< const FUniqueNetId > UserId)
{
    //@TODO
    /*WaitMessageWidget = SNew(SMahjongWaitDialog)
        .MessageText(NSLOCTEXT("NetworkStatus", "CheckingPrivilegesWithServer", "Checking privileges with server.  Please wait..."));*/

    if (GEngine && GEngine->GameViewport)
    {
        UGameViewportClient* const GVC = GEngine->GameViewport;
        //@TODO
        //GVC->AddViewportWidgetContent(WaitMessageWidget.ToSharedRef());
    }

    IOnlineIdentityPtr Identity = Online::GetIdentityInterface();
    if (Identity.IsValid() && UserId.IsValid())
    {
        Identity->GetUserPrivilege(*UserId, Privilege, Delegate);
    }
    else
    {
        // Can only get away with faking the UniqueNetId here because the delegates don't use it
        Delegate.ExecuteIfBound(FUniqueNetIdString(), Privilege, (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures);
    }
}

void UMahjongGameInstance::CleanupOnlinePrivilegeTask()
{
    //@TODO
    /*if (GEngine && GEngine->GameViewport && WaitMessageWidget.IsValid())
    {
        UGameViewportClient* const GVC = GEngine->GameViewport;
        GVC->RemoveViewportWidgetContent(WaitMessageWidget.ToSharedRef());
    }*/
}

void UMahjongGameInstance::DisplayOnlinePrivilegeFailureDialogs(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
    // Show warning that the user cannot play due to age restrictions
    //@TODO
    //UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GetGameViewportClient());
    TWeakObjectPtr<ULocalPlayer> OwningPlayer;
    if (GEngine)
    {
        for (auto It = GEngine->GetLocalPlayerIterator(GetWorld()); It; ++It)
        {
            TSharedPtr<const FUniqueNetId> OtherId = (*It)->GetPreferredUniqueNetId();
            if (OtherId.IsValid())
            {
                if (UserId == (*OtherId))
                {
                    OwningPlayer = *It;
                }
            }
        }
    }


    //@TODO
    //if (MahjongViewport != nullptr && OwningPlayer.IsValid())
    if (OwningPlayer.IsValid())
    {
        if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::AccountTypeFailure) != 0)
        {
            IOnlineExternalUIPtr ExternalUI = Online::GetExternalUIInterface();
            if (ExternalUI.IsValid())
            {
                ExternalUI->ShowAccountUpgradeUI(UserId);
            }
        }
        else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::RequiredSystemUpdate) != 0)
        {
            //@TODO
            /*MahjongViewport->ShowDialog(
                OwningPlayer.Get(),
                EMahjongDialogType::Generic,
                NSLOCTEXT("OnlinePrivilegeResult", "RequiredSystemUpdate", "A required system update is available.  Please upgrade to access online features."),
                NSLOCTEXT("DialogButtons", "OKAY", "OK"),
                FText::GetEmpty(),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric)
            );*/
        }
        else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::RequiredPatchAvailable) != 0)
        {
            //@TODO
            /*MahjongViewport->ShowDialog(
                OwningPlayer.Get(),
                EMahjongDialogType::Generic,
                NSLOCTEXT("OnlinePrivilegeResult", "RequiredPatchAvailable", "A required game patch is available.  Please upgrade to access online features."),
                NSLOCTEXT("DialogButtons", "OKAY", "OK"),
                FText::GetEmpty(),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric)
            );*/
        }
        else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::AgeRestrictionFailure) != 0)
        {
            //@TODO
            /*MahjongViewport->ShowDialog(
                OwningPlayer.Get(),
                EMahjongDialogType::Generic,
                NSLOCTEXT("OnlinePrivilegeResult", "AgeRestrictionFailure", "Cannot play due to age restrictions!"),
                NSLOCTEXT("DialogButtons", "OKAY", "OK"),
                FText::GetEmpty(),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric)
            );*/
        }
        else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::UserNotFound) != 0)
        {
            //@TODO
            /*MahjongViewport->ShowDialog(
                OwningPlayer.Get(),
                EMahjongDialogType::Generic,
                NSLOCTEXT("OnlinePrivilegeResult", "UserNotFound", "Cannot play due invalid user!"),
                NSLOCTEXT("DialogButtons", "OKAY", "OK"),
                FText::GetEmpty(),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric)
            );*/
        }
        else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::GenericFailure) != 0)
        {
            //@TODO
            /*MahjongViewport->ShowDialog(
                OwningPlayer.Get(),
                EMahjongDialogType::Generic,
                NSLOCTEXT("OnlinePrivilegeResult", "GenericFailure", "Cannot play online.  Check your network connection."),
                NSLOCTEXT("DialogButtons", "OKAY", "OK"),
                FText::GetEmpty(),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric),
                FOnClicked::CreateUObject(this, &UMahjongGameInstance::OnConfirmGeneric)
            );*/
        }
    }
}

void UMahjongGameInstance::OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result)
{
    FinishSessionCreation(Result);
}

void UMahjongGameInstance::FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        // Travel to the specified match URL
        GetWorld()->ServerTravel(TravelURL);
    }
    else
    {
        FText ReturnReason = NSLOCTEXT("NetworkErrors", "CreateSessionFailed", "Failed to create session.");
        FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
        ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
    }
}