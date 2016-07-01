// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongExec.h"
#include "MahjongPlayerController.h"
#include "MahjongGameMode.h"

UMahjongExec::UMahjongExec(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMahjongExec::Cheat(const FString& Message)
{
	AMahjongPlayerController* PlayerController = GetOuterAMahjongPlayerController();
	check(PlayerController);

	PlayerController->ServerCheat(Message.Left(128));
}

void UMahjongExec::AddTileToHand(int32 TileId)
{

}

void UMahjongExec::CalculateShanten()
{

}

void UMahjongExec::ForceGameStart()
{
	AMahjongPlayerController* PlayerController = GetOuterAMahjongPlayerController();
	check(PlayerController);

	AMahjongGameMode* GameMode = PlayerController->GetWorld()->GetAuthGameMode<AMahjongGameMode>();
	if (GameMode && GameMode->GetMatchState() == MatchState::WaitingToStart)
	{
		GameMode->StartMatch();
	}
}