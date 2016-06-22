// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"		// Required include
#include "MahjongGameController.h"	// Required include

#include "MahjongPlayerHand.h"


AMahjongGameController::AMahjongGameController() : Super()
{
}

void AMahjongGameController::BeginPlay() {

	PlayerHand = NewObject<UMahjongPlayerHand>(this);
	Super::BeginPlay();
}