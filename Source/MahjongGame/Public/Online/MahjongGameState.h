// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongGameState.generated.h"


UCLASS()
class AMahjongGameState : public AGameState {

	GENERATED_BODY()

protected:

	AMahjongGameState();

public:

	/** The time left for the player's turn. */
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	/** Whether the timer is paused. */
	UPROPERTY(Transient, Replicated)
	bool bTimerPaused;

	/** The score of each player. */
	UPROPERTY(Transient, Replicated)
	TArray<TPair<class AMahjongPlayerState*, int32>> PlayerScores;
};