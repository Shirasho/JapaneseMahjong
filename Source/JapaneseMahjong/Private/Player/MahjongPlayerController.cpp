// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"			// Required include
#include "MahjongPlayerController.h"	// Required Include

AMahjongPlayerController::AMahjongPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Hand;
}

void AMahjongPlayerController::BeginPlay() {
	Super::BeginPlay();
}