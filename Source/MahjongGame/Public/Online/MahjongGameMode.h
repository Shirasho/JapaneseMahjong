// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongGameMode.generated.h"

class AMahjongAIController;

UCLASS(config=Game)
class AMahjongGameMode : public AGameMode
{
	GENERATED_BODY()

protected:

	AMahjongGameMode();
	
protected:

	/** The AI pawn class. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
	TSubclassOf<APawn> AIPawnClass;

	/** The amount of time a player is allocated to take their turn (in seconds). */
	UPROPERTY(config)
	int32 PlayerTurnTime;

	//@TODO
	// This is where we would store the scores for each thing. This means we would probably
	// want to create a TMap that stores a hand and a han value.

	/** The array of active AI controllers. */
	UPROPERTY()
	TArray<AMahjongAIController*> AIControllers;

	/* The handle for the DefaultTimer timer. */
	FTimerHandle TimerHandle_DefaultTimer;

protected:

	/** Whether to create the exec manager (console command cheats) */
	virtual bool AllowCheats(APlayerController* P) override;

	/** Returns the game session class to use. */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

	/** Returns default pawn class for given controller */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	/** Determine who won the game. */
	virtual void DetermineGameWinner();
	
};
