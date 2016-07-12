// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongHUD.h"

#include "MahjongPlayerState.h"
#include "MahjongGameState.h"
#include "MahjongGameMode.h"
#include "MahjongPlayerController.h"

const float AMahjongHUD::MinHudScale = 0.5f;

AMahjongHUD::AMahjongHUD(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    WinFadeOutTime = 2.f;
    LastWinTime = -WinFadeOutTime;

    OnPlayerTalkingStateChangedDelegate = FOnPlayerTalkingStateChangedDelegate::CreateUObject(this, &AMahjongHUD::OnPlayerTalkingStateChanged);

    //@TODO
    //static ConstructorHelpers::FObjectFinder<UTexture2D> CH_HUDMainTexture(TEXT("/Game/UI/HUD/HUDMain"));
    //static ConstructorHelpers::FObjectFinder<UTexture2D> CH_HUDAssets02Texture(TEXT("/Game/UI/HUD/HUDAssets02"));
    //HUDMainTexture = CH_HUDMainTexture.Object;
    //HUDAssets02Texture = CH_HUDAssets02Texture.Object;

    // Fonts are not included in dedicated server builds.
#if !UE_SERVER
    {
        /*static ConstructorHelpers::FObjectFinder<UFont> CH_BigFont(TEXT("/Game/GUI/Fonts/####"));
        static ConstructorHelpers::FObjectFinder<UFont> CH_NormalFont(TEXT("/Game/GUI/Fonts/####"));
        BigFont = CH_BigFont.Object;
        NormalFont = CH_NormalFont.Object;*/
    }
#endif //!UE_SERVER

    //@TODO - Get the coordinates and the texture.
    GameTimerIcon = UCanvas::MakeIcon(HUDMainTexture, 262, 16, 255, 62);
    TurnTimerIcon = UCanvas::MakeIcon(HUDMainTexture, 262, 16, 255, 62);

    HUDLight = FColor(175, 202, 213, 255);
    HUDDark = FColor(110, 124, 131, 255);
    ShadowedFont.bEnableShadow = true;
}

void AMahjongHUD::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    bIsScoreBoardVisible = false;

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineVoicePtr VoiceInterface = OnlineSubsystem->GetVoiceInterface();
        if (VoiceInterface.IsValid())
        {
            VoiceInterface->AddOnPlayerTalkingStateChangedDelegate_Handle(OnPlayerTalkingStateChangedDelegate);
        }
    }
}

void AMahjongHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ConditionalCloseScoreboard(true);

    AMahjongPlayerController* PlayerController = Cast<AMahjongPlayerController>(PlayerOwner);
    if (PlayerController)
    {
        PlayerController->SetCinematicMode(false, false, false, true, true);
    }

    Super::EndPlay(EndPlayReason);
}

void AMahjongHUD::SetMahjongGameState(EMahjongGameState::Type NewState)
{
    MahjongGameState = NewState;
}

EMahjongGameState::Type AMahjongHUD::GetMahjongGameState() const
{
    return MahjongGameState;
}

FString AMahjongHUD::GetTimeString(float TimeSeconds)
{
    // only minutes and seconds are relevant
    const int32 TotalSeconds = FMath::Max(0, FMath::TruncToInt(TimeSeconds) % 3600);
    const int32 NumMinutes = TotalSeconds / 60;
    const int32 NumSeconds = TotalSeconds % 60;

    const FString TimeDesc = FString::Printf(TEXT("%02d:%02d"), NumMinutes, NumSeconds);
    return TimeDesc;
}

void AMahjongHUD::DrawGameTimer()
{
    //@TODO
}

void AMahjongHUD::DrawTurnTimer()
{
    AMahjongGameState* GameState = Cast<AMahjongGameState>(GetWorld()->GameState);
    Canvas->SetDrawColor(FColor::White);

    const float TimerPosX = Canvas->ClipX - Canvas->OrgX - (TurnTimerIcon.UL + Offset) * ScaleUI;
    const float TimerPosY = Canvas->OrgY + Offset * ScaleUI;

    if (GameState && MahjongGameState == EMahjongGameState::Playing && GameState->bTurnTimerUsed)
    {
        Canvas->DrawIcon(TurnTimerIcon, TimerPosX + Offset * ScaleUI, TimerPosY + (GameTimerIcon.VL / 2) * ScaleUI, ScaleUI);
    }

    if (GameState && GameState->RemainingTime > 0)
    {
        FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, HUDDark);
        TextItem.EnableShadow(FLinearColor::Black);

        float SizeX, SizeY;
        float TextScale = 0.57f;
        FString Text;

        TextItem.FontRenderInfo = ShadowedFont;
        TextItem.Scale = FVector2D(TextScale * ScaleUI, TextScale * ScaleUI);

        if (GameState->GetMatchState() == MatchState::WaitingToStart)
        {
            TextItem.Scale = FVector2D(ScaleUI, ScaleUI);
            Text = NSLOCTEXT("MahjongGame.HUD", "GameStartsIn", "PREPARING TABLE: ").ToString() + FString::FromInt(GameState->RemainingTime);
            TextItem.SetColor(HUDLight);
            TextItem.Text = FText::FromString(Text);
            AddGameInfoString(TextItem);
        }
        else if (GameState->GetMatchState() == MatchState::InProgress)
        {
            Text = GetTimeString(GameState->RemainingTime);
            Canvas->StrLen(BigFont, Text, SizeX, SizeY);

            TextItem.SetColor(HUDDark);
            TextItem.Text = FText::FromString(Text);
            TextItem.Position = FVector2D(TimerPosX + Offset * 1.5f * ScaleUI + TurnTimerIcon.UL * ScaleUI,
                                          TimerPosY + (TurnTimerIcon.VL * ScaleUI - SizeY * TextScale * ScaleUI)/2);
            Canvas->DrawItem(TextItem);
        }
    }
}

void AMahjongHUD::DrawHUD()
{
    Super::DrawHUD();
}

void AMahjongHUD::ConditionalCloseScoreboard(bool bFocus)
{
    if (bIsScoreBoardVisible)
    {
        ShowScoreboard(false, bFocus);
    }
}

void AMahjongHUD::ToggleScoreboard()
{
    ShowScoreboard(!bIsScoreBoardVisible);
}

bool AMahjongHUD::ShowScoreboard(bool bEnable, bool bFocus)
{
    if (bIsScoreBoardVisible == bEnable)
    {
        // Disable the scoreboard in favor of the new request.
        if (bEnable)
        {
            ToggleScoreboard();
        }
        else
        {
            return false;
        }
    }

    if (bEnable)
    {
        AMahjongPlayerController* PlayerController = Cast<AMahjongPlayerController>(PlayerOwner);
        if (!PlayerController || PlayerController->IsGameMenuVisible())
        {
            return false;
        }
    }

    bIsScoreBoardVisible = bEnable;
    if (bIsScoreBoardVisible)
    {
        //@TODO
        /*SAssignNew(ScoreboardWidgetOverlay, SOverlay)
            +SOverlay::Slot()
            .HAlign(EHorizontalAlignment::HAlign_Center)
            .VAlign(EVerticalAlignment::VAlign_Center)
            .Padding(FMargin(50))
            [
                SAssignNew(ScoreboardWidget, SMahjongScoreboardWidget)
                    .PlayerOwner(TWeakObjectPtr<APlayerController>(PlayerOwner))
                    .MahjongGameState(GetMahjongGameState())
            ];*/

        if (GEngine && GEngine->GameViewport)
        {
            GEngine->GameViewport->AddViewportWidgetContent(
                SAssignNew(ScoreboardWidgetContainer, SWeakWidget)
                .PossiblyNullContent(ScoreboardWidgetOverlay));
        }

        if (bFocus)
        {
            //@TODO
            //FSlateApplication::Get().SetKeyboardFocus(ScoreboardWidget);
        }
    }
    else
    {
        if (ScoreboardWidgetContainer.IsValid() && GEngine && GEngine->GameViewport)
        {
            GEngine->GameViewport->RemoveViewportWidgetContent(ScoreboardWidgetContainer.ToSharedRef());
        }

        if (bFocus)
        {
            FSlateApplication::Get().SetAllUserFocusToGameViewport();
        }
    }

    return true;
}

void AMahjongHUD::OnPlayerTalkingStateChanged(TSharedRef<const FUniqueNetId> TalkingPlayerId, bool bIsTalking)
{
    if (bIsScoreBoardVisible)
    {
        //@TODO
        //ScoreboardWidget->StoreTalkingPlayerData(TalkingPlayerId->ToString(), bIsTalking);
    }
}

void AMahjongHUD::ShowWinMessage(class AMahjongPlayerState* WinnerPlayerState)
{
    const float MessageDuration = 5.f;

    if (GetWorld() && GetWorld()->GameState && GetWorld()->GameState->GameModeClass)
    {
        const AMahjongGameMode* const GameMode = GetWorld()->GameState->GameModeClass->GetDefaultObject<AMahjongGameMode>();
        const AMahjongPlayerState* MyPlayerState = PlayerOwner ? Cast<AMahjongPlayerState>(PlayerOwner->PlayerState) : nullptr;

        if (GameMode && WinnerPlayerState && MyPlayerState)
        {
            FWinMessage NewMessage;
            NewMessage.WinnerName = WinnerPlayerState->GetShortPlayerName();
            NewMessage.bWinnerIsOwner = MyPlayerState == WinnerPlayerState;

            NewMessage.HideTime = GetWorld()->GetTimeSeconds() + MessageDuration;

            WinMessage = NewMessage;
            CenteredWinMessage = FText::FromString(NewMessage.WinnerName);
        }
    }
}

void AMahjongHUD::ToggleChat()
{
    AMahjongPlayerController* PlayerController = Cast<AMahjongPlayerController>(PlayerOwner);
    if (!PlayerController || PlayerController->IsGameMenuVisible() || GetMahjongGameState() == EMahjongGameState::Warmup)
    {
        return;
    }

    if (!TryCreateChatWidget())
    {
        //@TODO
        //EVisibility RequiredVisibility = ChatWidget->GetEntryVisibility() == EVisibility::Visible ? EVisibility::Hidden : EVisibility::Visible;
        //SetChatVisibility(RequiredVisibility);
    }
}

void AMahjongHUD::SetChatVisibility(const EVisibility RequiredVisibility)
{
    TryCreateChatWidget();

    //@TODO
    //if (ChatWidget->GetEntryVisibility() == RequiredVisibility)
    {
        return;
    }

    //@TODO
    //ChatWidget->SetEntryVisibility(RequiredVisibility);
}

void AMahjongHUD::AddChatLine(const FText& ChatString, bool bWantFocus)
{
    TryCreateChatWidget();
    if (ChatWidget.IsValid())
    {
        //@TODO
        //ChatWidget->AddChatLine(ChatString, bWantFocus);
    }
}

void AMahjongHUD::MakeUV(FCanvasIcon& Icon, FVector2D& UV0, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL)
{
    if (Icon.Texture)
    {
        const float Width = Icon.Texture->GetSurfaceWidth();
        const float Height = Icon.Texture->GetSurfaceHeight();
        UV0 = FVector2D(U / Width, V / Height);
        UV1 = UV0 + FVector2D(UL / Width, VL / Height);
    }
}

bool AMahjongHUD::TryCreateChatWidget()
{
    bool bCreated = false;

    AMahjongPlayerController* PlayerController = Cast<AMahjongPlayerController>(PlayerOwner);
    if (!PlayerController)
    {
        UE_LOG(LogOnlineChat, Warning, TEXT("Unable to create chat widget - Invalid player controller."))
    }
    else
    {
        if (!ChatWidget.IsValid())
        {
            bCreated = true;
            FLocalPlayerContext WorldContext(PlayerController);
            if (GEngine && GEngine->GameViewport)
            {
                //@TODO
                /*GEngine->GameViewport->AddViewportWidgetContent(
                    SAssignNew(ChatWidget, SChatWidget, WorldContext)
                );*/
            }
        }
    }
    return bCreated;
}

bool AMahjongHUD::IsGameOver() const
{
    return GetMahjongGameState() == EMahjongGameState::Lost ||
           GetMahjongGameState() == EMahjongGameState::Won;
}

void AMahjongHUD::AddGameInfoString(const FCanvasTextItem InInfoItem)
{
    InfoItems.Add(InInfoItem);
}

float AMahjongHUD::ShowInfoItems(float YOffset, float TextScale)
{
    float Y = YOffset;
    float CanvasCenter = Canvas->ClipX / 2.0f;

    for (int32 iItem = 0; iItem < InfoItems.Num(); iItem++)
    {
        float X = 0.0f;
        float SizeX, SizeY;
        Canvas->StrLen(InfoItems[iItem].Font, InfoItems[iItem].Text.ToString(), SizeX, SizeY);
        X = CanvasCenter - (SizeX * InfoItems[iItem].Scale.X) / 2.0f;
        Canvas->DrawItem(InfoItems[iItem], X, Y);
        Y += SizeY * InfoItems[iItem].Scale.Y;
    }
    return Y;
}