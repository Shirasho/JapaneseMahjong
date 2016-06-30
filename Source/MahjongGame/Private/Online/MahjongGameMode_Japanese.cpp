// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameMode_Japanese.h"

#include "MahjongGameState.h"


AMahjongGameMode_Japanese::AMahjongGameMode_Japanese() : Super() {
	
	// Whether the game should immediately start when the first player logs in. Affects the default behavior of ReadyToStartMatch.
	bDelayedStart = true;
}

void AMahjongGameMode_Japanese::DetermineGameWinner() {

	const AMahjongGameState* MyGameState = CastChecked<AMahjongGameState>(GameState);

	int32 BestPlayer = -1;
	int32 BestScore = MIN_FLT;

	// For all players
	for (int32 i = 0; i < MyGameState->PlayerArray.Num(); ++i) {

		// Get the score of the player.
		float PlayerScore = MyGameState->PlayerArray[i]->GetScore();

		// If their score is the highest
		if (PlayerScore > BestScore) {
			// Set their score as the highest and mark their index.
			BestScore = PlayerScore;
			BestPlayer = i;
		}
	}

	check(BestPlayer != -1);
	WinnerPlayerState = Cast<AMahjongPlayerState>(MyGameState->PlayerArray[BestPlayer]);
}