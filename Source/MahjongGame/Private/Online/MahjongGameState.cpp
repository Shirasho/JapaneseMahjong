// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameState.h"


AMahjongGameState::AMahjongGameState(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	RemainingTime = 0;
	bTimerPaused = false;
}

void AMahjongGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMahjongGameState, RemainingTime);
	DOREPLIFETIME(AMahjongGameState, bTimerPaused);
	DOREPLIFETIME(AMahjongGameState, PlayerScores);
}