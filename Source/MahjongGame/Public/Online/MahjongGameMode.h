// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongGameMode.generated.h"

class AMahjongAIController;

UCLASS(config=Game)
class AMahjongGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()
	
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

public:
    
    /* ## AGameMode Interface ## */

	/** Whether to create the exec manager (console command cheats) */
	virtual bool AllowCheats(APlayerController* P) override;

	/** Returns the game session class to use. */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

	/** Returns default pawn class for given controller */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

public:

    /* ## AMahjongGameMode ## */

	/** Determine who won the game. */
	virtual void DetermineGameWinner();

    /* Finish the game. */
    virtual void FinishGame();

    /* Finish the game and bump everyone to main menu.*/
    /* Only GameInstance should call this function. */
    void RequestFinishAndExitToMainMenu();

protected:

    virtual bool IsWinner(class AMahjongPlayerState* PlayerState) const;

public:

    /* ## Getters and Setters ## */

    virtual EMahjongGameMode GetGameMode() const;
};
