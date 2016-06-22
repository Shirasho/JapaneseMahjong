// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"		// Required include
#include "MahjongPlayerHand.h"		// Required include

#include "MahjongTile.h"
#include "JapaneseMahjongHandResolver.h"

UMahjongPlayerHand::UMahjongPlayerHand() {

	Tiles.Reserve(13);
	// TODO: Set this value to the max
	// number of possible tiles. There are
	// three rows, and I think each has 8 tiles in it.
	DiscardTiles.Reserve(24);

	HandResolver = new FJapaneseMahjongHandResolver();
}

void UMahjongPlayerHand::BeginDestroy() {

	delete HandResolver;

	Super::BeginDestroy();
}

void UMahjongPlayerHand::AddTile(AMahjongTile* Tile) {
	Tiles.Emplace(Tile);
}

void UMahjongPlayerHand::SetDraw(AMahjongTile* Tile) {
	DrawTile = Tile;
}

void UMahjongPlayerHand::Sort() {
	Tiles.Sort();
}

TArray<AMahjongTile*> UMahjongPlayerHand::GetHand() const {
	TArray<AMahjongTile*> ReturnTiles = TArray<AMahjongTile*>(Tiles);
	if (DrawTile) {
		ReturnTiles.Add(DrawTile);
	}
	return ReturnTiles;
}

bool UMahjongPlayerHand::ContainsTile(TEnumAsByte<ETileId> SpecificTile) const {

	for (const AMahjongTile* Tile : Tiles) {
		if (Tile->GetTileId() == SpecificTile) {
			return true;
		}
	}

	return false;
}
bool UMahjongPlayerHand::ContainsDragonTile(bool bAllDragonTileTypes) const {

	uint8 ContainedDragons = 0;

	for (const AMahjongTile* Tile : Tiles) {
		if (!bAllDragonTileTypes) {
			if (Tile->GetTileKind() == ETileKind::DRAGON) {
				return true;
			}
		} else {
			if (Tile->GetTileId() == ETileId::DRAGON_CHUN) {
				ContainedDragons |= (1 << 0);
			} else if (Tile->GetTileId() == ETileId::DRAGON_HAKU) {
				ContainedDragons |= (1 << 1);
			} else if (Tile->GetTileId() == ETileId::DRAGON_HATSU) {
				ContainedDragons |= (1 << 2);
			}
		}
	}

	return bAllDragonTileTypes ? (ContainedDragons == 7) : false;
}
bool UMahjongPlayerHand::ContainsWindTile(bool bAllWindTileTypes) const {

	uint8 ContainedWinds = 0;

	for (const AMahjongTile* Tile : Tiles) {
		if (!bAllWindTileTypes) {
			if (Tile->GetTileKind() == ETileKind::WIND) {
				return true;
			}
		}
		else {
			if (Tile->GetTileId() == ETileId::WIND_NAN) {
				ContainedWinds |= (1 << 0);
			}
			else if (Tile->GetTileId() == ETileId::WIND_PEI) {
				ContainedWinds |= (1 << 1);
			}
			else if (Tile->GetTileId() == ETileId::WIND_SHA) {
				ContainedWinds |= (1 << 2);
			}
			else if (Tile->GetTileId() == ETileId::WIND_TON) {
				ContainedWinds |= (1 << 3);
			}
		}
	}

	return bAllWindTileTypes ? (ContainedWinds == 15) : false;
}

FString UMahjongPlayerHand::ToString() const {
	FString Result;
	for (const AMahjongTile* Tile : Tiles) {
		Result += TEXT("{ " + Tile->GetTileName().ToString() + " }");
	}
	return Result;
}

FText UMahjongPlayerHand::ToText() const {
	FString Value;
	for (const AMahjongTile* Tile : Tiles) {
		Value += TEXT("{ " + Tile->GetTileName().ToString() + " }");
	}
	return FText::FromString(Value);
}