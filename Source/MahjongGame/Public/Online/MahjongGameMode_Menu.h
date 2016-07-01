// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongGameMode_Menu.generated.h"

UCLASS()
class AMahjongGameMode_Menu : public AGameMode {
	
	GENERATED_UCLASS_BODY()

public:

	/** Menu doesn't require player start or pawn */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	/** Returns game session class to use */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;
};