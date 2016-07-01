// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongLocalPlayer.generated.h"

UCLASS(config=Engine, transient)
class UMahjongLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:

	UMahjongLocalPlayer();

public:

	/** Sets the player's controller Id. */
	virtual void SetControllerId(int32 NewControllerId) override;

	/** Gets the player's handle. */
	virtual FString GetNickname() const;

	/** Gets the player's save game. */
	class UMahjongSaveGame* GetSaveGame() const;

	/** Loads the player's save game. */
	void LoadSaveGame();

private:

	/** Save game data stored between sessions. */
	UPROPERTY()
	class UMahjongSaveGame* SaveGame;
};
