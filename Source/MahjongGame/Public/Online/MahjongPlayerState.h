// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongTypes.h"
#include "MahjongPlayerState.generated.h"


UCLASS()
class AMahjongPlayerState : public APlayerState {

	GENERATED_BODY()

protected:

	AMahjongPlayerState();

private:

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

	/** Lose points. */
	void ScoreLoss(AMahjongPlayerState* Player, int32 PointLoss);

	/** gets truncated player name to fit in logs and scoreboards */
	FString GetShortPlayerName() const;

	/** Sends kill (excluding self) to clients */
	//UFUNCTION(Reliable, Client)
	//void InformAboutKill(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState);

	/** broadcast death to local clients */
	//UFUNCTION(Reliable, NetMulticast)
	//void BroadcastDeath(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState);

protected:

	/** The player's wind. */
	UPROPERTY(Transient, Replicated)
	EMahjongWindId PlayerWind;

	/** Whether the player quit the game. */
	UPROPERTY()
	bool bQuitter;
};