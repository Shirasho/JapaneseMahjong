// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameState.h"

#include "MahjongGameMode.h"
#include "MahjongGameInstance.h"
#include "MahjongPlayerState.h"
#include "MahjongPlayerController.h"


AMahjongGameState::AMahjongGameState(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	RemainingTime = 0;
    bTurnTimerUsed = true;
	bTimerPaused = false;
}

void AMahjongGameState::RequestFinishAndExitToMainMenu()
{
    if (AuthorityGameMode)
    {
        // We are the server with access to the GameMode.
        AMahjongGameMode* GameMode = Cast<AMahjongGameMode>(AuthorityGameMode);
        if (GameMode)
        {
            GameMode->RequestFinishAndExitToMainMenu();
        }
    }
    else
    {
        // We are a client with no access to GameMode. Just handle our own business.
        UMahjongGameInstance* GameInstance = Cast<UMahjongGameInstance>(GetGameInstance());
        if (GameInstance)
        {
            GameInstance->RemoveSplitScreenPlayers();
        }

        AMahjongPlayerController* PlayerController = Cast<AMahjongPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
        if (PlayerController)
        {
            // Safety check to ensure this is only being hit by the clients.
            check(PlayerController->GetNetMode() == ENetMode::NM_Client);
            PlayerController->ReturnToMainMenu();
        }
    }
}

void AMahjongGameState::PopulateRankedPlayerMap(RankedPlayerMap& OutRankedMap) const
{
    OutRankedMap.Empty();

    // Go over the PlayerStates, get their score, and rank them.
    TMultiMap<int32, AMahjongPlayerState*> SortedMap;
    for (int32 i = 0; i < PlayerArray.Num(); ++i)
    {
        int32 Score = 0;
        AMahjongPlayerState* CurrentPlayerState = Cast<AMahjongPlayerState>(PlayerArray[i]);
        if (CurrentPlayerState)
        {
            SortedMap.Add(FMath::TruncToInt(CurrentPlayerState->Score), CurrentPlayerState);
        }
    }

    // Sort by keys.
    SortedMap.KeySort(TGreater<int32>());

    int32 Rank = 0;
    for (TMultiMap<int32, AMahjongPlayerState*>::TIterator It(SortedMap); It; ++It)
    {
        OutRankedMap.Add(Rank++, It.Value());
    }
}

void AMahjongGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMahjongGameState, RemainingTime);
	DOREPLIFETIME(AMahjongGameState, bTimerPaused);
	DOREPLIFETIME(AMahjongGameState, PlayerScores);
}