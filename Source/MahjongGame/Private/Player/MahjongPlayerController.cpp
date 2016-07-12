// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongPlayerController.h"

#include "MahjongPlayerCameraManager.h"
#include "MahjongExec.h"
#include "MahjongHUD.h"
#include "MahjongGameInstance.h"
#include "MahjongGameViewportClient.h"
#include "MahjongPlayerState.h"
#include "MahjongLocalPlayer.h"
#include "MahjongSaveGame.h"


AMahjongPlayerController::AMahjongPlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
//@TODO	PlayerCameraManagerClass = AMahjongPlayerManagerClass::StaticClass();
    CheatClass = UMahjongExec::StaticClass();
    bAllowGameActions = true;
    bGameEndedFrame = false;

    ServerSayString = TEXT("Say");
    bHasSentStartEvents = false;
}

void AMahjongPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    //InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AMahjongPlayerController::OnToggleInGameMenu);
    //InputComponent->BindAction("Scoreboard", IE_Pressed, this, &AMahjongPlayerController::OnShowScoreboard);
    //InputComponent->BindAction("Scoreboard", IE_Released, this, &AMahjongPlayerController::OnHideScoreboard);

    InputComponent->BindAction("PushToTalk", IE_Pressed, this, &APlayerController::StartTalking);
    InputComponent->BindAction("PushToTalk", IE_Released, this, &APlayerController::StopTalking);

    //InputComponent->BindAction("ToggleChat", IE_Pressed, this, &AShooterPlayerController::ToggleChatWindow);
}

void AMahjongPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    //FMahjongStyle::Initialize();
}

void AMahjongPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AMahjongPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
    Super::TickActor(DeltaTime, TickType, ThisTickFunction);

    if (bGameEndedFrame)
    {
        bGameEndedFrame = false;

        if (IsPrimaryPlayer())
        {
            //AMahjongHUD* HUD = GetMahjongHUD();
            //if (HUD)
            {
                //	HUD->ShowScoreboard(true, true);
            }
        }
    }
}

void AMahjongPlayerController::SetPlayer(UPlayer* InPlayer)
{
    Super::SetPlayer(InPlayer);

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
}

void AMahjongPlayerController::UnFreeze()
{
    ServerRestartPlayer();
}

void AMahjongPlayerController::PawnPendingDestroy(APawn* InPawn)
{
    FVector CameraLocation = InPawn->GetActorLocation();
    FRotator CameraRotation = InPawn->GetActorRotation();

    Super::PawnPendingDestroy(InPawn);

    ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

void AMahjongPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
    UpdateSaveFileOnGameEnd(bIsWinner);

    Super::GameHasEnded(EndGameFocus, bIsWinner);
}

void AMahjongPlayerController::ClientSetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation)
{
    SetInitialLocationAndRotation(CameraLocation, CameraRotation);
    SetViewTarget(this);
}

bool AMahjongPlayerController::ServerCheat_Validate(const FString& Message)
{
    return true;
}

void AMahjongPlayerController::ServerCheat_Implementation(const FString& Message)
{
    if (CheatManager)
    {
        ClientMessage(ConsoleCommand(Message));
    }
}

void AMahjongPlayerController::ClientGameStarted_Implementation()
{
    bAllowGameActions = true;

    SetIgnoreMoveInput(false);
    bGameEndedFrame = false;

    // Send round start event.
    const auto Events = Online::GetEventsInterface();
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

    if (LocalPlayer && Events.IsValid())
    {
        auto UniqueId = LocalPlayer->GetPreferredUniqueNetId();
        if (UniqueId.IsValid())
        {
            // Generate a new session Id.
            Events->SetPlayerSessionId(*UniqueId, FGuid::NewGuid());

            FString MapName = *FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName());

            // Fire session start event.
            FOnlineEventParms Params;
            Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); //@TODO Get game mode id from game mode.
            Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); //@TODO Get AI difficulty if AI are present.
            Params.Add(TEXT("MapName"), FVariantData(MapName));

            Events->TriggerEvent(*UniqueId, TEXT("PlayerSessionStart"), Params);

            // Online matches require the MultiplayerRoundStart event.

            UMahjongGameInstance* GameInstance = GetWorld() ? Cast<UMahjongGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
            if (GameInstance && GameInstance->GetIsOnline())
            {
                FOnlineEventParms MultiplayerParams;

                MultiplayerParams.Add(TEXT("SectionId"), FVariantData((int32)0)); // Unused
                MultiplayerParams.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); //@TODO Get game mode id from game mode.
                MultiplayerParams.Add(TEXT("MatchTypeId"), FVariantData((int32)1)); // @TODO Abstract the specific meaning of this value across platforms
                MultiplayerParams.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); //@TODO Get AI difficulty if AI are present.

                Events->TriggerEvent(*UniqueId, TEXT("MultiplayerRoundStart"), MultiplayerParams);
            }

            bHasSentStartEvents = true;
        }
    }
}

void AMahjongPlayerController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
{
    Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

    // Disable controls.
    SetIgnoreMoveInput(true);
    bAllowGameActions = false;

    // Ensure valid view target.
    SetViewTarget(GetPawn());

    UpdateSaveFileOnGameEnd(bIsWinner);

    // Flag that the game has just ended.
    // If the game ended due to host loss we want to wait for return to menu implementation first.
    bGameEndedFrame = true;
}

void AMahjongPlayerController::ClientStartOnlineGame_Implementation()
{
    if (!IsPrimaryPlayer())
    {
        return;
    }

    AMahjongPlayerState* MahjongPlayerState = Cast<AMahjongPlayerState>(PlayerState);
    if (MahjongPlayerState)
    {
        IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
        if (OnlineSubsystem)
        {
            IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
            if (Sessions.IsValid())
            {
                UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *MahjongPlayerState->SessionName.ToString());
                Sessions->StartSession(MahjongPlayerState->SessionName);
            }
        }
    }
    else
    {
        // Keep trying until the player state is replicated.
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &AMahjongPlayerController::ClientStartOnlineGame_Implementation, 0.2f, false);
    }
}

void AMahjongPlayerController::ClientEndOnlineGame_Implementation()
{
    if (!IsPrimaryPlayer())
    {
        return;
    }

    AMahjongPlayerState* MahjongPlayerState = Cast<AMahjongPlayerState>(PlayerState);
    if (MahjongPlayerState)
    {
        IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
        if (OnlineSubsystem)
        {
            IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
            if (Sessions.IsValid())
            {
                UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *MahjongPlayerState->SessionName.ToString());
                Sessions->EndSession(MahjongPlayerState->SessionName);
            }
        }
    }
}

void AMahjongPlayerController::ClientSendGameEndEvent_Implementation(bool bIsWinner, int32 ExpendedTimeInSeconds)
{
    const auto Events = Online::GetEventsInterface();
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

    if (bHasSentStartEvents && LocalPlayer && Events.IsValid())
    {
        auto UniqueId = LocalPlayer->GetPreferredUniqueNetId();
        if (UniqueId.IsValid())
        {
            FString MapName = *FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName());
            AMahjongPlayerState* MahjongPlayerState = Cast<AMahjongPlayerState>(PlayerState);
            int32 PlayerScore = MahjongPlayerState ? MahjongPlayerState->GetScore() : 0;

            // Fire session end event.
            FOnlineEventParms Params;
            Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); //@TODO Get game mode id from game mode.
            Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); //@TODO Get AI difficulty if AI are present.
            Params.Add(TEXT("ExitStatusId"), FVariantData((int32)0)); // Unused
            Params.Add(TEXT("PlayerScore"), FVariantData(PlayerScore));
            Params.Add(TEXT("PlayerWon"), FVariantData(bIsWinner));
            Params.Add(TEXT("MapName"), FVariantData(MapName));
            Params.Add(TEXT("MapNameString"), FVariantData(MapName)); // @todo workaround for a bug in backend service, remove when fixed

            Events->TriggerEvent(*UniqueId, TEXT("PlayerSessionEnd"), Params);

            // Online matches require the MultiplayerRoundStart event.

            UMahjongGameInstance* GameInstance = GetWorld() ? Cast<UMahjongGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
            if (GameInstance && GameInstance->GetIsOnline())
            {
                FOnlineEventParms MultiplayerParams;

                MultiplayerParams.Add(TEXT("SectionId"), FVariantData((int32)0)); // Unused
                MultiplayerParams.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); //@TODO Get game mode id from game mode.
                MultiplayerParams.Add(TEXT("MatchTypeId"), FVariantData((int32)1)); // @TODO Abstract the specific meaning of this value across platforms
                MultiplayerParams.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); //@TODO Get AI difficulty if AI are present.
                MultiplayerParams.Add(TEXT("TimeInSeconds"), FVariantData(ExpendedTimeInSeconds));
                Params.Add(TEXT("ExitStatusId"), FVariantData((int32)0)); // Unused

                Events->TriggerEvent(*UniqueId, TEXT("MultiplayerRoundEnd"), MultiplayerParams);
            }
        }

        bHasSentStartEvents = false;
    }
}

void AMahjongPlayerController::ClientTeamMessage_Implementation(APlayerState* SendingPlayerState, const FString& Message, FName Type, float MsgLifeTime)
{
    //AMahjongHUD* MahjongHUD = Cast<AMahjongHUD>(GetHUD());
    //if (MahjongHUD)
    {
        if (Type == ServerSayString)
        {
            if (SendingPlayerState != PlayerState)
            {
                // MahjongHUD->AddChatLine(FText::FromString(Message), false);
            }
        }
    }
}

void AMahjongPlayerController::Say(const FString& Message)
{
    ServerSay(Message.Left(128));
}

bool AMahjongPlayerController::ServerSay_Validate(const FString& Message)
{
    return true;
}

void AMahjongPlayerController::ServerSay_Implementation(const FString& Message)
{
    GetWorld()->GetAuthGameMode()->Broadcast(this, Message, ServerSayString);
}

UMahjongSaveGame* AMahjongPlayerController::GetSaveGame() const
{
    UMahjongLocalPlayer* LocalPlayer = Cast<UMahjongLocalPlayer>(Player);
    return LocalPlayer ? LocalPlayer->GetSaveGame() : nullptr;
}

bool AMahjongPlayerController::IsGameInputAllowed() const
{
    return bAllowGameActions && !bCinematicMode;
}

void AMahjongPlayerController::UpdateSaveFileOnGameEnd(bool bIsWinner)
{
    AMahjongPlayerState* MahjongPlayerState = Cast<AMahjongPlayerState>(PlayerState);
    if (MahjongPlayerState)
    {

        UMahjongSaveGame* SaveGame = GetSaveGame();
        if (SaveGame)
        {
            //@TODO - Get game mode.
            SaveGame->AddGameResult(EMahjongGameMode::INVALID, MahjongPlayerState->GetScore(), bIsWinner);
            SaveGame->SaveIfDirty();
        }
    }
}

void AMahjongPlayerController::OnWin()
{
    //@TODO Achievements here.

    ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(Player);
    if (LocalPlayer)
    {
        const IOnlineEventsPtr EventInterface = Online::GetEventsInterface();
        const IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface();

        if (EventInterface.IsValid() && IdentityInterface.IsValid())
        {
            const int32 UserIndex = LocalPlayer->GetControllerId();
            TSharedPtr<const FUniqueNetId> UniqueId = IdentityInterface->GetUniquePlayerId(UserIndex);

            if (UniqueId.IsValid())
            {
                FOnlineEventParms Params;
                Params.Add(TEXT("SectionId"), FVariantData((int32)0)); // Unused
                Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); //@TODO Get game mode.
                Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); //@TODO Get bot difficulty.

                Params.Add(TEXT("PlayerRoleId"), FVariantData((int32)0)); // unused
                Params.Add(TEXT("PlayerWeaponId"), FVariantData((int32)0)); // unused
                Params.Add(TEXT("EnemyRoleId"), FVariantData((int32)0)); // unused
                Params.Add(TEXT("EnemyWeaponId"), FVariantData((int32)0)); // unused

                Params.Add(TEXT("LocationX"), FVariantData((int32)0)); // unused
                Params.Add(TEXT("LocationY"), FVariantData((int32)0)); // unused
                Params.Add(TEXT("LocationZ"), FVariantData((int32)0)); // unused

                EventInterface->TriggerEvent(*UniqueId, TEXT("PlayerWin"), Params);
            }
        }
    }
}

void AMahjongPlayerController::OnWinMessage(class AMahjongPlayerState* WinnerPlayerState)
{
    AMahjongHUD* const MahjongHUD = Cast<AMahjongHUD>(GetHUD());
    if (MahjongHUD)
    {
        MahjongHUD->ShowWinMessage(WinnerPlayerState);
    }

    ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(Player);
    if (LocalPlayer && LocalPlayer->GetCachedUniqueNetId().IsValid() && WinnerPlayerState->UniqueId.IsValid())
    {
        // If this controller is the player who won, update their stats.
        if (*LocalPlayer->GetCachedUniqueNetId() == *WinnerPlayerState->UniqueId)
        {
            const IOnlineEventsPtr EventInterface = Online::GetEventsInterface();
            const IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface();

            if (EventInterface.IsValid() && IdentityInterface.IsValid())
            {
                const int32 UserIndex = LocalPlayer->GetControllerId();
                TSharedPtr<const FUniqueNetId> UniqueId = IdentityInterface->GetUniquePlayerId(UserIndex);

                if (UniqueId.IsValid())
                {
                    FOnlineEventParms Params;
                    Params.Add(TEXT("SectionId"), FVariantData((int32)0)); // Unused
                    Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); //@TODO Get game mode.
                    Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); //@TODO Get bot difficulty.

                    Params.Add(TEXT("PlayerRoleId"), FVariantData((int32)0)); // unused
                    Params.Add(TEXT("PlayerWeaponId"), FVariantData((int32)0)); // unused
                    Params.Add(TEXT("EnemyRoleId"), FVariantData((int32)0)); // unused
                    Params.Add(TEXT("EnemyWeaponId"), FVariantData((int32)0)); // unused

                    Params.Add(TEXT("LocationX"), FVariantData((int32)0)); // unused
                    Params.Add(TEXT("LocationY"), FVariantData((int32)0)); // unused
                    Params.Add(TEXT("LocationZ"), FVariantData((int32)0)); // unused

                    EventInterface->TriggerEvent(*UniqueId, TEXT("PlayerWin"), Params);
                }
            }
        }
    }
}

void AMahjongPlayerController::ShowInGameMenu()
{
    AMahjongHUD* MahjongHUD = Cast<AMahjongHUD>(GetHUD());
    //@TODO
    /*if (MahjongInGameMenu.IsValid() && !MahjongInGameMenu->GetIsGameMenuUp() && MahjongHUD && !MahjongHUD->IsGameOver())
    {
        MahjongInGameMenu->ToggleGameMenu();
    }*/
}

bool AMahjongPlayerController::IsGameMenuVisible() const
{
    bool Result = false;
    //@TODO
    /*if (MahjongInGameMenu.IsValid())
    {
        Result = MahjongIngameMenu->GetIsGameMenuUp();
    }*/

    return Result;
}

void AMahjongPlayerController::ReturnToMainMenu()
{
    //@TODO OnHideScoreboard();
    CleanupSessionOnReturnToMenu();
}

void AMahjongPlayerController::CleanupSessionOnReturnToMenu()
{
    UMahjongGameInstance * GameInstance = GetWorld() ? Cast<UMahjongGameInstance>(GetWorld()->GetGameInstance()) : nullptr;

    if (ensure(GameInstance))
    {
        GameInstance->CleanupSessionOnReturnToMenu();
    }
}