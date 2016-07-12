// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongPlayerState.h"
#include "MahjongPlayerScoreContainer.generated.h"

USTRUCT()
struct FMahjongPlayerScoreContainer
{

    GENERATED_BODY()

    UPROPERTY()
    AMahjongPlayerState* PlayerState;

    void SetScore(int32 Value)
    {
        if (IsValid())
        {
            PlayerState->Score = Value;
        }
    }

    void AddScore(int32 Value)
    {
        if (IsValid())
        {
            PlayerState->Score += Value;
            // Ensure float accuracy.
            PlayerState->Score = FMath::Max(FMath::RoundToInt(PlayerState->Score), 0);
        }
    }

    void SubtractScore(int32 Value)
    {
        if (IsValid())
        {
            PlayerState->Score -= Value;
            // Ensure float accuracy.
            PlayerState->Score = FMath::Max(FMath::RoundToInt(PlayerState->Score), 0);
        }
    }

    bool IsValid() const
    {
        if (!PlayerState)
        {
            return false;
        }
        return PlayerState->IsValidLowLevel();
    }

    FMahjongPlayerScoreContainer()
    {
        PlayerState = nullptr;
    }
};