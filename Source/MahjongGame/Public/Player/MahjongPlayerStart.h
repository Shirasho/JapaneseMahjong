// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongTypes.h"
#include "MahjongPlayerStart.generated.h"

UCLASS()
class AMahjongPlayerStart : public APlayerStart
{
    GENERATED_UCLASS_BODY()

public:

    /** What wind spawns at this point */
    UPROPERTY(EditInstanceOnly, Category = Position)
    EMahjongPlayerWind SpawnWind;
};