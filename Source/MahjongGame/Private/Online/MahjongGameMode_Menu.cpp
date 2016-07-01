// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameMode_Menu.h"

#include "MahjongPlayerController_Menu.h"
#include "MahjongGameSession.h"

AMahjongGameMode_Menu::AMahjongGameMode_Menu(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer) {

	PlayerControllerClass = AMahjongPlayerController_Menu::StaticClass();
}

void AMahjongGameMode_Menu::RestartPlayer(AController* NewPlayer) {
	// Do nothing
}

TSubclassOf<AGameSession> AMahjongGameMode_Menu::GetGameSessionClass() const {
	return AMahjongGameSession::StaticClass();
}