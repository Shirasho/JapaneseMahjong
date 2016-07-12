// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongPendingMessage.h"
#include "MahjongPendingInvite.h"

#include "MahjongGameInstance.generated.h"

class FMahjongMainMenu;
class FMahjongWelcomeMenu;
class FMahjongMessageMenu;
class SMahjongWaitWidget;
class AMahjongGameSession;


namespace MahjongGameInstanceState
{
    extern const FName None;
    extern const FName PendingInvite;
    extern const FName WelcomeScreen;
    extern const FName MainMenu;
    extern const FName MessageMenu;
    extern const FName Playing;
}


UCLASS(config=Game)
class UMahjongGameInstance : public UGameInstance
{
    GENERATED_UCLASS_BODY()

private:

    /** The map the game loads into on consoles. */
    UPROPERTY(config)
    FString WelcomeScreenMap;

    /** The map the game loads into on PCs. Also contains the main menu map. */
    UPROPERTY(config)
    FString MainMenuMap;

    /** The current game state. */
    FName CurrentState;

    /** The pending game state. */
    FName PendingState;

    FMahjongPendingMessage PendingMessage;
    FMahjongPendingInvite PendingInvite;

    /** URL to travel to after pending network operations */
    FString TravelURL;

    /** Whether the match is online or not */
    bool bIsOnline;

    /** Whether the user has an active license to play the game */
    bool bIsLicensed;

    /** If true, enable splitscreen when map starts loading */
    bool bPendingEnableSplitscreen;


    /** Main menu UI */
    TSharedPtr<FMahjongMainMenu> MainMenu;

    /** Message menu (Shown in the event of errors - unable to connect etc) */
    TSharedPtr<FMahjongMessageMenu> MessageMenu;

    /** Welcome menu UI (for consoles) */
    TSharedPtr<FMahjongWelcomeMenu> WelcomeMenu;

    /** Dialog widget to show non-interactive waiting messages for network timeouts and such. */
    TSharedPtr<SMahjongWaitWidget> WaitMessageWidget;


    /** Controller to ignore for pairing changes. -1 to skip ignore. */
    int32 IgnorePairingChangeForControllerId;

    /** Last connection status that was passed into the HandleNetworkConnectionStatusChanged hander */
    EOnlineServerConnectionStatus::Type	CurrentConnectionStatus;

    /** Delegate for callbacks to Tick */
    FTickerDelegate TickDelegate;

    /** Handle to various registered delegates */
    FDelegateHandle TickDelegateHandle;
    FDelegateHandle TravelLocalSessionFailureDelegateHandle;
    FDelegateHandle OnJoinSessionCompleteDelegateHandle;
    FDelegateHandle OnSearchSessionsCompleteDelegateHandle;
    FDelegateHandle OnStartSessionCompleteDelegateHandle;
    FDelegateHandle OnEndSessionCompleteDelegateHandle;
    FDelegateHandle OnDestroySessionCompleteDelegateHandle;
    FDelegateHandle OnCreatePresenceSessionCompleteDelegateHandle;

    /** Delegate for ending a session */
    FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;

    /** Asset loader for async requests. */
    FStreamableManager AssetLoader;

public:

    bool Tick(float DeltaSeconds);

    AMahjongGameSession* GetGameSession() const;

    /** Returns true if the game is in online mode */
    FORCEINLINE bool GetIsOnline() const { return bIsOnline; }

    FORCEINLINE FStreamableManager& GetAssetLoader() { return AssetLoader; }

    /** Sets the online mode of the game */
    void SetIsOnline(bool bInIsOnline);

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void StartGameInstance() override;

    bool HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL);
    bool JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults);
    bool JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult);

    void BeginHostingGame(int32 Map);

    /** Travel directly to the named session */
    void TravelToSession(const FName& SessionName);

    /** Initiates the session searching */
    bool FindSessions(ULocalPlayer* PlayerOwner, bool bLANMatch);

    /** Sends the game to the specified state. */
    void GotoState(FName NewState);

    /** Obtains the initial welcome state, which can be different based on platform */
    FName GetInitialState();

    /** Sends the game to the initial startup/frontend state  */
    void GotoInitialState();

    void RemoveSplitScreenPlayers();

    /**
    * Creates the message menu, clears other menus and sets the KingState to Message.
    *
    * @param	Message				Main message body
    * @param	OKButtonString		String to use for 'OK' button
    * @param	CancelButtonString	String to use for 'Cancel' button
    * @param	NewState			Final state to go to when message is discarded
    */
    void ShowMessageThenGotoState(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString, const FName& NewState, const bool OverrideExisting = true, TWeakObjectPtr< ULocalPlayer > PlayerOwner = nullptr);

    void RemoveExistingLocalPlayer(ULocalPlayer* ExistingPlayer);

    TSharedPtr< const FUniqueNetId > GetUniqueNetIdFromControllerId(const int ControllerId);

    /** Sets the controller to ignore for pairing changes. Useful when we are showing external UI for manual profile switching. */
    void SetIgnorePairingChangeForControllerId(const int32 ControllerId);

    /** Returns true if the passed in local player is signed in and online */
    bool IsLocalPlayerOnline(ULocalPlayer* LocalPlayer);

    /** Returns true if owning player is online. Displays proper messaging if the user can't play */
    bool ValidatePlayerForOnlinePlay(ULocalPlayer* LocalPlayer);

    /** Shuts down the session, and frees any net driver */
    void CleanupSessionOnReturnToMenu();

    /** Flag the local player when they quit the game */
    void LabelPlayerAsQuitter(ULocalPlayer* LocalPlayer) const;

    // Generic confirmation handling (just hide the dialog)
    FReply OnConfirmGeneric();

    bool HasLicense() const { return bIsLicensed; }

    /** Start task to get user privileges. */
    void StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate, EUserPrivileges::Type Privilege, TSharedPtr< const FUniqueNetId > UserId);

    /** Common cleanup code for any Privilege task delegate */
    void CleanupOnlinePrivilegeTask();

    /** Show approved dialogs for various privileges failures */
    void DisplayOnlinePrivilegeFailureDialogs(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);

private:

    virtual void OnSessionUserInviteAccepted(
        const bool							bWasSuccess,
        const int32							ControllerId,
        TSharedPtr< const FUniqueNetId >	UserId,
        const FOnlineSessionSearchResult &	InviteResult
    ) override;

    void HandleNetworkConnectionStatusChanged(EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus);

    void HandleSessionFailure(const FUniqueNetId& NetId, ESessionFailure::Type FailureType);

    void OnPreLoadMap();
    void OnPostLoadMap();

    /** Delegate function executed after checking privileges for starting quick match */
    void OnUserCanPlayInvite(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);

    void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);

    void MaybeChangeState();
    void EndCurrentState(FName NextState);
    void BeginNewState(FName NewState, FName PrevState);

    void BeginPendingInviteState();
    void BeginWelcomeScreenState();
    void BeginMainMenuState();
    void BeginMessageMenuState();
    void BeginPlayingState();

    void EndPendingInviteState();
    void EndWelcomeScreenState();
    void EndMainMenuState();
    void EndMessageMenuState();
    void EndPlayingState();

    void ShowLoadingScreen();
    /**
    * Creates the message menu, clears other menus and sets the KingState to Message.
    *
    * @param	Message				Main message body
    * @param	OKButtonString		String to use for 'OK' button
    * @param	CancelButtonString	String to use for 'Cancel' button
    */
    void ShowMessageThenGoMain(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString);

    void AddNetworkFailureHandlers();
    void RemoveNetworkFailureHandlers();

    /** Called when there is an error trying to travel to a local session */
    void TravelLocalSessionFailure(UWorld *World, ETravelFailure::Type FailureType, const FString& ErrorString);

    /** Callback which is intended to be called upon joining session */
    void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);

    /** Callback which is intended to be called upon session creation */
    void OnCreatePresenceSessionComplete(FName SessionName, bool bWasSuccessful);

    /** Callback which is called after adding local users to a session */
    void OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);

    /** Called after all the local players are registered */
    void FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result);

    /** Callback which is called after adding local users to a session we're joining */
    void OnRegisterJoiningLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);

    /** Called after all the local players are registered in a session we're joining */
    void FinishJoinSession(EOnJoinSessionCompleteResult::Type Result);

    /** Callback which is intended to be called upon finding sessions */
    void OnSearchSessionsComplete(bool bWasSuccessful);

    bool LoadFrontEndMap(const FString& MapName);

    /** Sets a rich presence string for all local players. */
    void SetPresenceForLocalPlayers(const FVariantData& PresenceData);

    /** Travel directly to the named session */
    void InternalTravelToSession(const FName& SessionName);

    // OSS delegates to handle
    void HandleUserLoginChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId);

    // Callback to pause the game when the OS has constrained our app.
    void HandleAppWillDeactivate();

    // Callback occurs when game being suspended
    void HandleAppSuspend();

    // Callback occurs when game resuming
    void HandleAppResume();

    // Callback to process game licensing change notifications.
    void HandleAppLicenseUpdate();

    // Callback to handle safe frame size changes.
    void HandleSafeFrameChanged();

    // Callback to handle controller connection changes.
    void HandleControllerConnectionChange(bool bIsConnection, int32 Unused, int32 GameUserIndex);

    /** Show messaging and punt to welcome screen */
    void HandleSignInChangeMessaging();

    // Callback to handle controller pairing changes.
    FReply OnPairingUsePreviousProfile();

    // Callback to handle controller pairing changes.
    FReply OnPairingUseNewProfile();

    // Callback to handle controller pairing changes.
    void HandleControllerPairingChanged(int GameUserIndex, const FUniqueNetId& PreviousUser, const FUniqueNetId& NewUser);

    // Handle confirming the controller disconnected dialog.
    FReply OnControllerReconnectConfirm();

protected:
        
    bool HandleOpenCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* InWorld);
};