// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongSaveGame.h"

#include "MahjongLocalPlayer.h"
#include "MathBlueprintLibrary.h"


UMahjongSaveGame::UMahjongSaveGame(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetToDefaults();
}

void UMahjongSaveGame::SetToDefaults()
{
	bIsDirty = false;

	Gamma = 2.2f;
}

void UMahjongSaveGame::SetGamma(float InGamma) {
    bIsDirty |= Gamma != InGamma;
    Gamma = InGamma;
}

uint32 UMahjongSaveGame::GetGameScoreAverage(EMahjongGameMode GameMode) const
{
	return GameScoreAverage.FindRef(GameMode);
}

uint32 UMahjongSaveGame::GetGameCount(EMahjongGameMode GameMode) const
{
	return GameCount.FindRef(GameMode);
}

uint32 UMahjongSaveGame::GetGameWins(EMahjongGameMode GameMode) const
{
	return GameWins.FindRef(GameMode);
}

uint32 UMahjongSaveGame::GetGameLeaves(EMahjongGameMode GameMode) const
{
	return GameLeaves.FindRef(GameMode);
}

float UMahjongSaveGame::GetWinPercentage(EMahjongGameMode GameMode) const
{
	return (float)UMathBlueprintLibrary::SetPrecision_Double((double)GetGameWins(GameMode) / (double)GetGameCount(GameMode), 2);
}

void UMahjongSaveGame::Save()
{
	UGameplayStatics::SaveGameToSlot(this, SlotName, UserIndex);
	bIsDirty = false;
}

UMahjongSaveGame* UMahjongSaveGame::LoadSaveGame(FString SlotName, int32 UserIndex)
{
	UMahjongSaveGame* Result = nullptr;

	// First player sign-in can happen before UWorld exists, which means no
	// online subsystem, which means no usernames, which means no slot names.
	// Save games are not valid in this state.
	if (SlotName.Len() > 0)
	{
		Result = Cast<UMahjongSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
		if (!Result)
		{
			Result = Cast<UMahjongSaveGame>(UGameplayStatics::CreateSaveGameObject(UMahjongSaveGame::StaticClass()));
		}
		check(Result);

		Result->SlotName = SlotName;
		Result->UserIndex = UserIndex;
	}

	return Result;
}

void UMahjongSaveGame::SaveIfDirty()
{
	if (bIsDirty)
	{
		Save();
	}
}

void UMahjongSaveGame::AddGameResult(EMahjongGameMode GameMode, int32 Score, bool bIsGameWinner)
{
	// Update the number of games played in this mode.
	uint32 GameModeCount = ++(GameCount.FindOrAdd(GameMode));
	// Update the number of wins in this mode.
	if (bIsGameWinner)
	{
		++(GameWins.FindOrAdd(GameMode));
	}
	// Update the total score earned in this game mode.
	uint64 GameModeScore = GameScores.FindOrAdd(GameMode) += Score;
	// Update the average score earned in this game mode.
	GameScoreAverage.FindOrAdd(GameMode) = GameModeScore / (uint64)GameModeCount;

	bIsDirty = true;
}

void UMahjongSaveGame::AddGameLeave(EMahjongGameMode GameMode)
{
	++(GameLeaves.FindOrAdd(GameMode));

	bIsDirty = true;
}

void UMahjongSaveGame::FetchKeyBindings()
{
	TArray<APlayerController*> PlayerList;
	if (GEngine)
	{
		GEngine->GetAllLocalPlayerControllers(PlayerList);
	}

	for (APlayerController* Controller : PlayerList)
	{
		if (!Controller || !Controller->Player || !Controller->PlayerInput)
		{
			continue;
		}

        // Update key bindings for current user only.
        UMahjongLocalPlayer* LocalPlayer = Cast<UMahjongLocalPlayer>(Controller->Player);
        if (!LocalPlayer || LocalPlayer->GetSaveGame() != this)
        {
            continue;
        }

        // Update bindings.
        /*int32 AxisMapCount = Controller->PlayerInput->AxisMappings.Num();
        for (int32 AxisMapIndex = 0; AxisMapIndex < AxisMapCount; ++AxisMapIndex)
        {
            FInputAxisKeyMapping& AxisMapping = PC->PlayerInput->AxisMappings[AxisMapIndex];
            if (AxisMapping.AxisName == "")
            {
                AxisMapping.Scale = 1;
            }
        }
        Controller->PlayerInput->ForceRebuildingKeyMaps();*/
	}
}