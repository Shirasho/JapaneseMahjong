// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"	// Required include
#include "MahjongWanTile.h"		// Required include


// Sets default values
AMahjongWanTile::AMahjongWanTile() : Super()
{

}

void AMahjongWanTile::Init(uint8 TileValue, bool bRedTile) {
	check(TileValue >= 1);
	check(TileValue <= 9);

	Super::Init(TileValue, bRedTile);

	TileId = (ETileId)(static_cast<uint8>(ETileId::WAN_1) + TileValue - 1);
}