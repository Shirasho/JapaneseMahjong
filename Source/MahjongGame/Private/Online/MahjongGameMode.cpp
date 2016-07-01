// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameMode.h"

#include "MahjongPlayerController.h"
#include "MahjongPlayerState.h"
#include "MahjongGameState.h"
#include "MahjongGameSession.h"

AMahjongGameMode::AMahjongGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> CH_PlayerPawn(TEXT("/Game/Blueprints/Pawns/PlayerPawn"));
	static ConstructorHelpers::FClassFinder<APawn> CH_AIPawn(TEXT("/Game/Blueprints/Pawns/AIPawn"));

	DefaultPawnClass = CH_PlayerPawn.Class;
	AIPawnClass = CH_AIPawn.Class;

	//@TODO
	//HUDClass = AMahjongHUD::StaticClass();
	PlayerControllerClass = AMahjongPlayerController::StaticClass();
	PlayerStateClass = AMahjongPlayerState::StaticClass();
	//Spectatorclass = AMahjongSpectatorPawn::StaticClass();
	GameStateClass = AMahjongGameState::StaticClass();
	//ReplaySpectatorPlayerControllerClass = AMahjongReplaySpectator::StaticClass();
}

TSubclassOf<AGameSession> AMahjongGameMode::GetGameSessionClass() const
{
	return AMahjongGameSession::StaticClass();
}

bool AMahjongGameMode::AllowCheats(APlayerController* P)
{
	return true;
}

UClass* AMahjongGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<AMahjongAIController>())
	{
		return AIPawnClass;
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AMahjongGameMode::DetermineGameWinner()
{
	// Do nothing.
}