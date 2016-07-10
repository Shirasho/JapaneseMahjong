// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongTypes.h"
#include "MahjongTilePool.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMahjongTilePool : public AActor
{
    GENERATED_UCLASS_BODY()

private:

    UPROPERTY(replicated)
    TArray<uint8> Tiles;

public:

    void BeginPlay() override;
    void Tick(float DeltaTime) override;

};