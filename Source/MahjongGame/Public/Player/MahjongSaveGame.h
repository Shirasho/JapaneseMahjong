// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongTypes.h"
#include "MahjongSaveGame.generated.h"

UCLASS()
class UMahjongSaveGame : public USaveGame {


	GENERATED_BODY()

protected:

	UPROPERTY()
	TMap<EMahjongGameMode, uint32> GameWins;

	UPROPERTY()
	TMap<EMahjongGameMode, uint32> GameLeaves;

	UPROPERTY()
	TMap<EMahjongGameMode, uint32> GameCount;

	UPROPERTY()
	TMap<EMahjongGameMode, uint64> GameScores;

	UPROPERTY()
	TMap<EMahjongGameMode, uint32> GameScoreAverage;

	UPROPERTY()
	float Gamma;


private:

	bool bIsDirty;
	FString SlotName;
	int32 UserIndex;

public:

	/** Loads a save game if it exists, creates an empty record otherwise. */
	static UMahjongSaveGame* LoadSaveGame(FString SlotName, int32 UserIndex);

	/** Saves data to the slot if anything has changed. */
	void SaveIfDirty();

	/** Records the result of a game */
	void AddGameResult(EMahjongGameMode GameMode, int32 Score, bool bIsGameWinner);

	/** Records a game leave (the player left or disconnected before the game finished). */
	void AddGameLeave(EMahjongGameMode GameMode);

	/** Fetch key bindings and tell input subsystem about them. */
	void FetchKeyBindings();

	/** Gets the average score gained in the specified game mode. */
	uint32 GetGameScoreAverage(EMahjongGameMode GameMode) const;

	/** Gets the number of games played in the specified game mode. */
	uint32 GetGameCount(EMahjongGameMode GameMode) const;

	/** Gets the number of wins in the specified game mode. */
	uint32 GetGameWins(EMahjongGameMode GameMode) const;

	/** Gets the number of times the player has left the specified game mode. */
	uint32 GetGameLeaves(EMahjongGameMode GameMode) const;

	float GetWinPercentage(EMahjongGameMode GameMode) const;


	void SetGamma(float InGamma);

	FORCEINLINE FString GetName() const	{ return SlotName; }
	FORCEINLINE float GetGamma() const
{ return Gamma; }
	FORCEINLINE int32 GetUserIndex() const
{ return UserIndex; }

protected:

	void SetToDefaults();

	void Save();
};