// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongPlayerScoreContainer.h"
#include "MahjongGameState.generated.h"

// Ranked PlayerState map, created from GameState.
typedef TMap<int32, TWeakObjectPtr<AMahjongPlayerState>> RankedPlayerMap;

UCLASS()
class AMahjongGameState : public AGameState {

	GENERATED_UCLASS_BODY()

public:

	/** The time left for the player's turn. */
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

    /** Whether the turn timer is required for this game. */
    UPROPERTY(Transient, Replicated)
    bool bTurnTimerUsed;

	/** Whether the timer is paused. */
	UPROPERTY(Transient, Replicated)
	bool bTimerPaused;

	/** The score of each player. */
	UPROPERTY(Transient, Replicated)
	TArray<FMahjongPlayerScoreContainer> PlayerScores;

public:

    void RequestFinishAndExitToMainMenu();

    // Populates the map in the order of the player's score.
    void PopulateRankedPlayerMap(RankedPlayerMap& OutRankedPlayerMap) const;
};