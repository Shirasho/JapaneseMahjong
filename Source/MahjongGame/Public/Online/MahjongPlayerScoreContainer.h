// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongPlayerState.h"
#include "MahjongPlayerScoreContainer.generated.h"

USTRUCT()
struct FMahjongPlayerScoreContainer {

	GENERATED_BODY()

	UPROPERTY()
	AMahjongPlayerState* PlayerState;

	UPROPERTY()
	uint32 Score;

	void SetScore(uint32 Value) {
		Score = Value;
	}

	void AddScore(uint32 Value) {
		Score += Value;
	}

	void SubtractScore(uint32 Value) {
		if ((int32)Score - (int32)Value < 0) {
			Score = 0;
		} else {
			Score -= Value;
		}
	}

	bool IsValid() const {
		if (!PlayerState) return false;
		return PlayerState->IsValidLowLevel();
	}

	const AMahjongPlayerState* GetPlayerState() const {
		return PlayerState;
	}

	FMahjongPlayerScoreContainer() {
		Score = 0;
		PlayerState = nullptr;
	}
};