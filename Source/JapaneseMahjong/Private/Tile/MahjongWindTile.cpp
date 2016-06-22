// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"	// Required include
#include "MahjongWindTile.h"	// Required include


// Sets default values
AMahjongWindTile::AMahjongWindTile() : Super()
{

}

void AMahjongWindTile::Init(uint8 TileValue, bool bRedTile) {
	check(TileValue >= 1);
	check(TileValue <= 4);

	Super::Init(TileValue, bRedTile);

	TileId = (ETileId)(static_cast<uint8>(ETileId::WIND_TON) + TileValue - 1);
}