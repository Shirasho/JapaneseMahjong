// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongExec.generated.h"

UCLASS(Within=MahjongPlayerController)
class UMahjongExec : public UCheatManager {

	GENERATED_UCLASS_BODY()

	UFUNCTION(exec)
	void Cheat(const FString& Message);

	UFUNCTION(exec)
	void AddTileToHand(int32 TileId);

	UFUNCTION(exec)
	void CalculateShanten();

	UFUNCTION(exec)
	void ForceGameStart();
};