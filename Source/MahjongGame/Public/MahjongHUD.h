// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongTypes.h"
#include "MahjongHUD.generated.h"

struct FWinMessage
{
    /** Name of player who the game. */
    FString WinnerName;

    /** Killer is local player. */
    bool bWinnerIsOwner;

    /** Timestamp for removing message */
    float HideTime;

    /** Initialise defaults. */
    FWinMessage()
        : bWinnerIsOwner(false)
        , HideTime(0.f)
    {
    }
};

UCLASS()
class AMahjongHUD : public AHUD
{
    GENERATED_UCLASS_BODY()

public:

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Main HUD update loop. */
    virtual void DrawHUD() override;

    void SetMahjongGameState(EMahjongGameState::Type NewState);

    /** Get state of current match. */
    EMahjongGameState::Type GetMahjongGameState() const;

    /** Turns off scoreboard if it is being displayed */
    void ConditionalCloseScoreboard(bool bFocus = false);

    /** Toggles scoreboard */
    void ToggleScoreboard();

    /**
    * Toggles in game scoreboard.
    * Note:Will not display if the game menu is visible.

    * @param	bEnable	Required scoreboard display state.
    * @param	bFocus	Give keyboard focus to the scoreboard.
    * @return	true, if the scoreboard visibility changed
    */
    bool ShowScoreboard(bool bEnable, bool bFocus = false);

    void ShowWinMessage(class AMahjongPlayerState* WinnerPlayerState);

    void ToggleChat();
    void SetChatVisibility(const EVisibility RequiredVisibility);
    void AddChatLine(const FText& ChatString, bool bWantFocus);

    bool IsGameOver() const;

    /** Draws turn timer. */
    void DrawTurnTimer();

    /** Draws game timer. */
    void DrawGameTimer();

    /** Draw the game winner. */
    void DrawGameWinner();

    /** Delegate for telling other methods when players have started/stopped talking */
    FOnPlayerTalkingStateChangedDelegate OnPlayerTalkingStateChangedDelegate;
    void OnPlayerTalkingStateChanged(TSharedRef<const FUniqueNetId> TalkingPlayerId, bool bIsTalking);

    /*
    * Add information string that will be displayed on the hud. They are added as required and rendered together to prevent overlaps
    *
    * @param InInfoString	InInfoString
    */
    void AddGameInfoString(const FCanvasTextItem InfoItem);

    /*
    * Render the info messages.
    *
    * @param YOffset	YOffset from top of canvas to start drawing the messages
    * @param TextScale	Text scale factor
    *
    * @returns The next Y position to draw any further strings
    */
    float ShowInfoItems(float YOffset, float TextScale);

    /** helper for getting uv coords in normalized top,left, bottom, right format */
    void MakeUV(FCanvasIcon& Icon, FVector2D& UV0, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL);

    /** Temporary helper for drawing text-in-a-box. */
    void DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor);

protected:

    /** Called every time game is started. */
    virtual void PostInitializeComponents() override;

    /**
    * Converts floating point seconds to MM:SS string.
    *
    * @param TimeSeconds		The time to get a string for.
    */
    FString GetTimeString(float TimeSeconds);

    bool TryCreateChatWidget();

protected:

    /** Floor for automatic hud scaling. */
    static const float MinHudScale;

    /** Lighter HUD color. */
    FColor HUDLight;

    /** Darker HUD color. */
    FColor HUDDark;

    /** Game timer icon. */
    UPROPERTY()
    FCanvasIcon GameTimerIcon;

    /** Timer icon. */
    UPROPERTY()
    FCanvasIcon TurnTimerIcon;

    /** UI scaling factor for other resolutions than Full HD. */
    float ScaleUI;

    /** Current animation pulse value. */
    float PulseValue;

    /** FontRenderInfo enabling casting shadows. */
    FFontRenderInfo ShadowedFont;

    /* Big "RON! [PLAYER]" (or whatever win value) message text 
     * that appears in the center of the screen. */
    FText CenteredWinMessage;

    /** The time that somebody won. */
    float LastWinTime;

    /** How long the message will fade out. */
    float WinFadeOutTime;

    /** texture for HUD elements. */
    UPROPERTY()
    UTexture2D* HUDMainTexture;

    /** Texture for HUD elements. */
    UPROPERTY()
    UTexture2D* HUDAssets02Texture;

    UPROPERTY()
    UFont* BigFont;

    UPROPERTY()
    UFont* NormalFont;

    /** General offset for HUD elements. */
    float Offset;

    /** State of match. */
    EMahjongGameState::Type MahjongGameState;

    /** Is the scoreboard widget visible on screen? */
    bool bIsScoreBoardVisible;

    /** Scoreboard widget. */
    TSharedPtr<class SMahjongScoreboardWidget>	ScoreboardWidget;

    /** Scoreboard widget overlay. */
    TSharedPtr<class SOverlay>	ScoreboardWidgetOverlay;

    /** Scoreboard widget container - used for removing */
    TSharedPtr<class SWidget> ScoreboardWidgetContainer;

    /** Chatbox widget. */
    TSharedPtr<class SChatWidget> ChatWidget;

    /** Array of information strings to render. */
    TArray<FCanvasTextItem> InfoItems;

    /** The win message to display. */
    FWinMessage WinMessage;
};