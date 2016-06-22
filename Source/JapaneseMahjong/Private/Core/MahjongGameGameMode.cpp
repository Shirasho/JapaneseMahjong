// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"		// Required include
#include "MahjongGameGameMode.h"	// Required include
#include "MahjongGameController.h"	// Required include
#include "MahjongGameState.h"		// Required include

AMahjongGameGameMode::AMahjongGameGameMode() : Super() {

	DefaultPawnClass = nullptr;
	PlayerControllerClass = AMahjongGameController::StaticClass();
	GameStateClass = AMahjongGameState::StaticClass();
}

void AMahjongGameGameMode::BeginPlay() {
	Super::BeginPlay();
}