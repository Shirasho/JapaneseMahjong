// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameMode.h"

#include "MahjongAIController.h"
#include "MahjongPlayerController.h"
#include "MahjongPlayerState.h"
#include "MahjongGameState.h"
#include "MahjongGameSession.h"
#include "MahjongGameInstance.h"

AMahjongGameMode::AMahjongGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    // Whether the game should immediately start when the first player logs in. Affects the default behavior of ReadyToStartMatch.
    bDelayedStart = true;

	static ConstructorHelpers::FClassFinder<APawn> CH_PlayerPawn(TEXT("/Game/Blueprints/Pawns/PlayerPawn"));
	static ConstructorHelpers::FClassFinder<APawn> CH_AIPawn(TEXT("/Game/Blueprints/Pawns/AIPawn"));

	DefaultPawnClass = CH_PlayerPawn.Class;
	AIPawnClass = CH_AIPawn.Class;

	//@TODO
	//HUDClass = AMahjongHUD::StaticClass();
	PlayerControllerClass = AMahjongPlayerController::StaticClass();
	PlayerStateClass = AMahjongPlayerState::StaticClass();
	//Spectatorclass = AMahjongSpectatorPawn::StaticClass();
	GameStateClass = AMahjongGameState::StaticClass();
	//ReplaySpectatorPlayerControllerClass = AMahjongReplaySpectator::StaticClass();
}

EMahjongGameMode AMahjongGameMode::GetGameMode() const
{
    return EMahjongGameMode::INVALID;
}

TSubclassOf<AGameSession> AMahjongGameMode::GetGameSessionClass() const
{
	return AMahjongGameSession::StaticClass();
}

bool AMahjongGameMode::AllowCheats(APlayerController* P)
{
	return true;
}

UClass* AMahjongGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<AMahjongAIController>())
	{
		return AIPawnClass;
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AMahjongGameMode::DetermineGameWinner()
{
	// Do nothing.
}

void AMahjongGameMode::RequestFinishAndExitToMainMenu()
{
    FinishGame();

    UMahjongGameInstance* GameInstance = Cast<UMahjongGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        //@TODO For now we do not have split screen (due to the nature of Mahjong),
        // but if we did it would need to be removed here.

        //GameInstance->RemoveSplitScreenPlayers();
    }

    AMahjongPlayerController* PlayerController = nullptr;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AMahjongPlayerController* Controller = Cast<AMahjongPlayerController>(*It);
        
        if (!Controller)
        {
            continue;
        }

        // If the controller isn't a local controller, return them to the main menu with the specified return reason.
        if (!Controller->IsLocalController())
        {
            const FString RemoteReturnReason = NSLOCTEXT("NetworkErrors", "HostHasLeft", "Host has left the game.").ToString();
            Controller->ClientReturnToMainMenu(RemoteReturnReason);
        }
        else
        {
            PlayerController = Controller;
        }
    }

    if (PlayerController)
    {
        // Return the local player to the main menu.
        PlayerController->ReturnToMainMenu();
    }
}

void AMahjongGameMode::FinishGame()
{
    AMahjongGameState* MyGameState = Cast<AMahjongGameState>(GameState);
    
    if (IsMatchInProgress())
    {
        EndMatch();
        DetermineGameWinner();

        // Notify players that the game has finished.
        for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
        {
            AMahjongPlayerState* PlayerState = Cast<AMahjongPlayerState>((*It)->PlayerState);
            bool bIsWinner = IsWinner(PlayerState);

            (*It)->GameHasEnded(nullptr, bIsWinner);
        }

        // Turn off all Pawns.
        for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
        {
            (*It)->TurnOff();
        }

        // Reset the turn timer.
        MyGameState->RemainingTime = PlayerTurnTime;
    }
}

bool AMahjongGameMode::IsWinner(class AMahjongPlayerState* PlayerState) const
{
    return false;
}
