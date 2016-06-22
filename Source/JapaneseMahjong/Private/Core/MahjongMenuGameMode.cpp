// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"			// Required include
#include "MahjongMenuGameMode.h"		// Required include
#include "MahjongMenuController.h"		// Required include
#include "MahjongGameState.h"			// Required include

AMahjongMenuGameMode::AMahjongMenuGameMode() : Super() {

	DefaultPawnClass = nullptr;
	PlayerControllerClass = AMahjongMenuController::StaticClass();
	GameStateClass = AMahjongGameState::StaticClass();
}

void AMahjongMenuGameMode::BeginPlay() {
	Super::BeginPlay();
}