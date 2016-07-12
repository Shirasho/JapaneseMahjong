// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongPlayerState.h"

#include "PlayerDefinitions.h"

#include "MahjongGameState.h"
#include "MahjongPlayerController.h"


AMahjongPlayerState::AMahjongPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerWind = static_cast<uint8>(EMahjongPlayerWind::WIND_NONE);
	bQuitter = false;
}

void AMahjongPlayerState::Reset()
{
	Super::Reset();

	PlayerWind = static_cast<uint8>(EMahjongPlayerWind::WIND_NONE);
	bQuitter = false;
}

void AMahjongPlayerState::ClientInitialize(AController* InController)
{
	Super::ClientInitialize(InController);
}

void AMahjongPlayerState::SetQuitter(bool bInQuitter)
{
	bQuitter = bInQuitter;
}

bool AMahjongPlayerState::IsQuitter() const
{
	return bQuitter;
}

int32 AMahjongPlayerState::GetScore() const
{
	return Score;
}

FString AMahjongPlayerState::GetShortPlayerName() const
{
    if (PlayerName.Len() > PLAYER_NAME_LENGTH_MAX)
    {
        return PlayerName.Left(PLAYER_NAME_LENGTH_MAX) + TEXT("...");
    }

    return PlayerName;
}

void AMahjongPlayerState::ScoreWin(TArray<TPair<AMahjongPlayerState*, float>> FromArray)
{
	AMahjongGameState* MyGameState = Cast<AMahjongGameState>(GetWorld()->GameState);

	int32 ScoreToAdd = 0;

	if (MyGameState)
	{
        for (FMahjongPlayerScoreContainer& PlayerScoreContainer : MyGameState->PlayerScores)
        {
            //PlayerScoreContainer.AddScore(FromArray.FindByKey(PlayerScoreContainer.PlayerState)->Value);
            for (auto Pair : FromArray)
            {
                if (Pair.Key == PlayerScoreContainer.PlayerState)
                {
                    PlayerScoreContainer.AddScore(Pair.Value);
                    break;
                }
            }
        }
	}

	Score += ScoreToAdd;
}

void AMahjongPlayerState::InformAboutWin_Implementation(class AMahjongPlayerState* WinnerPlayerState)
{
    //Id can be null for bots
    if (WinnerPlayerState->UniqueId.IsValid())
    {
        //search for the actual killer before calling OnKill()	
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            AMahjongPlayerController* PlayerController = Cast<AMahjongPlayerController>(*It);
            if (PlayerController && PlayerController->IsLocalController())
            {
                // A local player might not have an ID if it was created with CreateDebugPlayer.
                ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);
                TSharedPtr<const FUniqueNetId> LocalID = LocalPlayer->GetCachedUniqueNetId();
                if (LocalID.IsValid() && *LocalPlayer->GetCachedUniqueNetId() == *WinnerPlayerState->UniqueId)
                {
                    PlayerController->OnWin();
                }
            }
        }
    }
}

void AMahjongPlayerState::BroadcastWin_Implementation(class AMahjongPlayerState* WinnerPlayerState)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        // all local players get death messages so they can update their huds.
        AMahjongPlayerController* PlayerController = Cast<AMahjongPlayerController>(*It);
        if (PlayerController && PlayerController->IsLocalController())
        {
            PlayerController->OnWinMessage(WinnerPlayerState);
        }
    }
}

void AMahjongPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMahjongPlayerState, PlayerWind);
}