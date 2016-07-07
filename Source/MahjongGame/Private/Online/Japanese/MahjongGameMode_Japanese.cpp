// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameMode_Japanese.h"

#include "MahjongPlayerState.h"
#include "MahjongGameState.h"
#include "MahjongGameSession_Japanese.h"


AMahjongGameMode_Japanese::AMahjongGameMode_Japanese(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    //static ConstructorHelpers::FClassFinder<APawn> CH_PlayerPawn(TEXT("/Game/Blueprints/Pawns/PlayerPawn"));
    //static ConstructorHelpers::FClassFinder<APawn> CH_AIPawn(TEXT("/Game/Blueprints/Pawns/AIPawn"));

    //@TODO
    //DefaultPawnClass = CH_PlayerPawn.Class;
    //AIPawnClass = CH_AIPawn.Class;
    //HUDClass = AMahjongHUD::StaticClass();
    //PlayerControllerClass = AMahjongPlayerController::StaticClass();
    //PlayerStateClass = AMahjongPlayerState::StaticClass();
    //Spectatorclass = AMahjongSpectatorPawn::StaticClass();
    //GameStateClass = AMahjongGameState::StaticClass();
    //ReplaySpectatorPlayerControllerClass = AMahjongReplaySpectator::StaticClass();
}

EMahjongGameMode AMahjongGameMode_Japanese::GetGameMode() const
{
    return EMahjongGameMode::JAPANESE;
}

TSubclassOf<AGameSession> AMahjongGameMode_Japanese::GetGameSessionClass() const
{
    return AMahjongGameSession_Japanese::StaticClass();
}

void AMahjongGameMode_Japanese::DetermineGameWinner()
{
	const AMahjongGameState* MyGameState = CastChecked<AMahjongGameState>(GameState);

	int32 BestPlayer = -1;
	int32 BestScore = INT_MIN;

	// For all players
	for (int32 i = 0; i < MyGameState->PlayerArray.Num(); ++i)
	{
		// Get the score of the player.
		float PlayerScore = MyGameState->PlayerArray[i]->Score;

		// If their score is the highest
		if (PlayerScore > BestScore)
		{
			// Set their score as the highest and mark their index.
			BestScore = PlayerScore;
			BestPlayer = i;
		}
	}

	check(BestPlayer != -1);
	WinnerPlayerState = Cast<AMahjongPlayerState>(MyGameState->PlayerArray[BestPlayer]);
}

bool AMahjongGameMode_Japanese::IsWinner(AMahjongPlayerState* PlayerState) const
{
    return PlayerState && !PlayerState->IsQuitter() && PlayerState == WinnerPlayerState;
}