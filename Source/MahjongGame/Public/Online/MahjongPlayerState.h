// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongTypes.h"
#include "MahjongPlayerState.generated.h"


UCLASS()
class AMahjongPlayerState : public APlayerState {

	GENERATED_UCLASS_BODY()

public:

	/** Clear the scores */
	virtual void Reset() override;

	/** Initialize the controller state */
	virtual void ClientInitialize(class AController* InController) override;

	/** Whether the player quit the game. */
	bool IsQuitter() const;

	/** Set whether the player is a quitter */
	void SetQuitter(bool bInQuitter);

	/** Get the player's score. */
	int32 GetScore() const;

	/** Score points. */
	void ScoreWin(TArray<TPair<AMahjongPlayerState*, float>> FromArray);

	/** gets truncated player name to fit in logs and scoreboards */
	FString GetShortPlayerName() const;

	/** Sends win (excluding self) to clients */
	UFUNCTION(Reliable, Client)
	void InformAboutWin(class AMahjongPlayerState* WinnerPlayerState);

	/** Broadcast win to local clients */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastWin(class AMahjongPlayerState* WinnerPlayerState);

    FORCEINLINE uint8 GetPlayerWind() { return PlayerWind; }
    FORCEINLINE bool GetQuitter() { return bQuitter; }

protected:

	/** The player's wind. */
	UPROPERTY(Transient, Replicated)
	uint8 PlayerWind;

	/** Whether the player quit the game. */
	UPROPERTY()
	bool bQuitter;
};