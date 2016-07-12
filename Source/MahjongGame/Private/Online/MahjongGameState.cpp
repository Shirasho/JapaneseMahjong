// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameState.h"

#include "MahjongGameMode.h"
#include "MahjongGameInstance.h"
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

void AMahjongGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMahjongGameState, RemainingTime);
	DOREPLIFETIME(AMahjongGameState, bTimerPaused);
	DOREPLIFETIME(AMahjongGameState, PlayerScores);
}