// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

class UMahjongGameInstance;
class SMahjongMenuWidget;
class SMahjongDialogWidget;

class FMahjongMainMenu : public TSharedFromThis<FMahjongMainMenu>, public FTickableGameObject
{

protected:

    enum class EMap
    {
        EBambooPark,
        EMax
    };

    TWeakObjectPtr<UMahjongGameInstance> GameInstance;
    TWeakObjectPtr<ULocalPlayer> PlayerOwner;

    TSharedPtr<class SMahjongMenuWidget> MenuWidget;
    TSharedPtr<class SWeakWidget> MenuWidgetContainer;

    TSharedPtr<class SMahjongDialogWidget> SearchingWidget;
    TSharedPtr<class SWeakWidget> SearchingWidgetContainer;

    TSharedPtr<class SMahjongDialogWidget> StoppingWidget;
    TSharedPtr<class SWeakWidget> StoppingWidgetContainer;

    /** Track if we are showing a map download pct or not. */
    bool bShowingDownloadPct;

    /** Are we currently animating the Searching for a Game UI? */
    bool bAnimateSearchingUI;

    /** Was the search request for a game canceled while searching? */
    bool bSearchRequestCancelled;

    /** Was input used to cancel the search request for quickmatch? */
    bool bUsedInputToCancelSearch;

    /** Custom animation var that is used to determine whether or not to inc or dec the alpha value of the quickmatch UI*/
    bool bIncrementAlpha;

    /** lan game? */
    bool bIsLanMatch;

    FDelegateHandle OnMatchmakingCompleteDelegateHandle;
    FDelegateHandle OnCancelMatchmakingCompleteDelegateHandle;

public:

    virtual ~FMahjongMainMenu();

    /** build menu */
    void Construct(TWeakObjectPtr<UMahjongGameInstance> InGameInstance, TWeakObjectPtr<ULocalPlayer> InPlayerOwner);

    /** Add the menu to the gameviewport so it becomes visible */
    void AddMenuToGameViewport();

    /** Remove from the gameviewport. */
    void RemoveMenuFromGameViewport();

    /** TickableObject Functions */
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    virtual bool IsTickableWhenPaused() const override;

    /** Returns the player that owns the main menu. */
    ULocalPlayer* GetPlayerOwner() const;

    /** Returns the controller id of player that owns the main menu. */
    int32 GetPlayerOwnerControllerId() const;

    /** Returns the string name of the currently selected map */
    FString GetMapName() const;

protected:

    EMap GetSelectedMap() const;

    /** Start the check for whether the owner of the menu has online privileges */
    void StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate);

    /** Common cleanup code for any Privilege task delegate */
    void CleanupOnlinePrivilegeTask();

    /** Delegate executed when matchmaking completes */
    FOnMatchmakingCompleteDelegate OnMatchmakingCompleteDelegate;

    /** Delegate function executed when the quick match async operation is complete */
    void OnMatchmakingComplete(FName SessionName, bool bWasSuccessful);

    /** Delegate for canceling matchmaking */
    FOnCancelMatchmakingCompleteDelegate OnCancelMatchmakingCompleteDelegate;

    /** Delegate function executed when the quick match async cancel operation is complete */
    void OnCancelMatchmakingComplete(FName SessionName, bool bWasSuccessful);

private:

    void MatchmakingSearchingUICancel(bool bShouldRemoveSession);
};
