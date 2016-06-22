// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"	// Required include
#include "MahjongDragonTile.h"	// Required include

// Sets default values
AMahjongDragonTile::AMahjongDragonTile() : Super()
{
	
}

void AMahjongDragonTile::Init(uint8 TileValue, bool bRedTile) {
	check(TileValue >= 1);
	check(TileValue <= 3);

	Super::Init(TileValue, bRedTile);

	TileId = (ETileId)(static_cast<uint8>(ETileId::DRAGON_HAKU) + TileValue - 1);
}