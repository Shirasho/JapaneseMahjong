// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongTilePool.h"

AMahjongTilePool::AMahjongTilePool(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    bReplicates = true;
    bAlwaysRelevant = true;
    bNetLoadOnClient = true;
}

void AMahjongTilePool::BeginPlay()
{

    Super::BeginPlay();
}

void AMahjongTilePool::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMahjongTilePool::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    DOREPLIFETIME(AMahjongTilePool, Tiles);
}