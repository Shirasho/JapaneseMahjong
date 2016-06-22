// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"	// Required include
#include "MahjongSouTile.h"		// Required include

// Sets default values
AMahjongSouTile::AMahjongSouTile() : Super()
{

}

void AMahjongSouTile::Init(uint8 TileValue, bool bRedTile) {
	check(TileValue >= 1);
	check(TileValue <= 9);

	Super::Init(TileValue, bRedTile);

	TileId = (ETileId)(static_cast<uint8>(ETileId::SOU_1) + TileValue - 1);
}