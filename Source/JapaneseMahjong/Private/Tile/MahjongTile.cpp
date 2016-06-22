// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"	// Required include
#include "UnrealNetwork.h"		// Replication

#include "MahjongPlayerController.h"
#include "MahjongTile.h"

FText AMahjongTile::InvalidTileName = NSLOCTEXT("Mahjong.Tiles", "InvalidTileName", "Invalid Tile");
TMap<ETileId, FText> AMahjongTile::TileNames = AMahjongTile::PopulateTileNames();

AMahjongTile::AMahjongTile() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
	bAlwaysRelevant = true;

	TileValue = -1;
	TileId = ETileId::INVALID;
}

void AMahjongTile::Init(uint8 TileValue, bool bRedTile) {
	check(TileValue > static_cast<uint8>(ETileId::INVALID));
	check(TileValue < static_cast<uint8>(ETileId::TILE_ID_MAX));

	this->TileValue = TileValue;
	this->bRedTile = bRedTile;
	this->TileOwner = nullptr;
	this->PrevTileOwner = nullptr;
}

void AMahjongTile::ResetTile() {
	TileOwner = nullptr;
}

// Called when the game starts or when spawned
void AMahjongTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMahjongTile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

ETileKind AMahjongTile::GetTileKind() const {
	return ETileKind::NONE;
}

AMahjongPlayerController* AMahjongTile::GetTileOwner() const {
	return TileOwner;
}

AMahjongPlayerController* AMahjongTile::GetPrevTileOwner() const {
	return PrevTileOwner;
}

FText AMahjongTile::GetTileName() const {
	FText TileName = TileNames.FindRef(TileId);
	return TileName.IsEmpty() ? InvalidTileName : TileName;
}

TMap<ETileId, FText> AMahjongTile::PopulateTileNames() {
	TMap<ETileId, FText> TileNames;
	
	TileNames.Emplace(ETileId::WAN_1) = NSLOCTEXT("Mahjong.Tile", "Wan1", "1 Wan");
	TileNames.Emplace(ETileId::WAN_2) = NSLOCTEXT("Mahjong.Tile", "Wan2", "2 Wan");
	TileNames.Emplace(ETileId::WAN_3) = NSLOCTEXT("Mahjong.Tile", "Wan3", "3 Wan");
	TileNames.Emplace(ETileId::WAN_4) = NSLOCTEXT("Mahjong.Tile", "Wan4", "4 Wan");
	TileNames.Emplace(ETileId::WAN_5) = NSLOCTEXT("Mahjong.Tile", "Wan5", "5 Wan");
	TileNames.Emplace(ETileId::WAN_6) = NSLOCTEXT("Mahjong.Tile", "Wan6", "6 Wan");
	TileNames.Emplace(ETileId::WAN_7) = NSLOCTEXT("Mahjong.Tile", "Wan7", "7 Wan");
	TileNames.Emplace(ETileId::WAN_8) = NSLOCTEXT("Mahjong.Tile", "Wan8", "8 Wan");
	TileNames.Emplace(ETileId::WAN_9) = NSLOCTEXT("Mahjong.Tile", "Wan9", "9 Wan");

	TileNames.Emplace(ETileId::PIN_1) = NSLOCTEXT("Mahjong.Tile", "Pin1", "1 Pin");
	TileNames.Emplace(ETileId::PIN_2) = NSLOCTEXT("Mahjong.Tile", "Pin2", "2 Pin");
	TileNames.Emplace(ETileId::PIN_3) = NSLOCTEXT("Mahjong.Tile", "Pin3", "3 Pin");
	TileNames.Emplace(ETileId::PIN_4) = NSLOCTEXT("Mahjong.Tile", "Pin4", "4 Pin");
	TileNames.Emplace(ETileId::PIN_5) = NSLOCTEXT("Mahjong.Tile", "Pin5", "5 Pin");
	TileNames.Emplace(ETileId::PIN_6) = NSLOCTEXT("Mahjong.Tile", "Pin6", "6 Pin");
	TileNames.Emplace(ETileId::PIN_7) = NSLOCTEXT("Mahjong.Tile", "Pin7", "7 Pin");
	TileNames.Emplace(ETileId::PIN_8) = NSLOCTEXT("Mahjong.Tile", "Pin8", "8 Pin");
	TileNames.Emplace(ETileId::PIN_9) = NSLOCTEXT("Mahjong.Tile", "Pin9", "9 Pin");

	TileNames.Emplace(ETileId::SOU_1) = NSLOCTEXT("Mahjong.Tile", "Sou1", "1 Sou");
	TileNames.Emplace(ETileId::SOU_2) = NSLOCTEXT("Mahjong.Tile", "Sou2", "2 Sou");
	TileNames.Emplace(ETileId::SOU_3) = NSLOCTEXT("Mahjong.Tile", "Sou3", "3 Sou");
	TileNames.Emplace(ETileId::SOU_4) = NSLOCTEXT("Mahjong.Tile", "Sou4", "4 Sou");
	TileNames.Emplace(ETileId::SOU_5) = NSLOCTEXT("Mahjong.Tile", "Sou5", "5 Sou");
	TileNames.Emplace(ETileId::SOU_6) = NSLOCTEXT("Mahjong.Tile", "Sou6", "6 Sou");
	TileNames.Emplace(ETileId::SOU_7) = NSLOCTEXT("Mahjong.Tile", "Sou7", "7 Sou");
	TileNames.Emplace(ETileId::SOU_8) = NSLOCTEXT("Mahjong.Tile", "Sou8", "8 Sou");
	TileNames.Emplace(ETileId::SOU_9) = NSLOCTEXT("Mahjong.Tile", "Sou9", "9 Sou");

	TileNames.Emplace(ETileId::WIND_TON) = NSLOCTEXT("Mahjong.Tile", "WindEast", "East Wind");
	TileNames.Emplace(ETileId::WIND_NAN) = NSLOCTEXT("Mahjong.Tile", "WindSouth", "South Wind");
	TileNames.Emplace(ETileId::WIND_SHA) = NSLOCTEXT("Mahjong.Tile", "WindWest", "West Wind");
	TileNames.Emplace(ETileId::WIND_PEI) = NSLOCTEXT("Mahjong.Tile", "WindNorth", "North Wind");

	TileNames.Emplace(ETileId::DRAGON_HAKU) = NSLOCTEXT("Mahjong.Tile", "DragonWhite", "White Dragon");
	TileNames.Emplace(ETileId::DRAGON_HATSU) = NSLOCTEXT("Mahjong.Tile", "DragonGreen", "Green Dragon");
	TileNames.Emplace(ETileId::DRAGON_CHUN) = NSLOCTEXT("Mahjong.Tile", "DragonRed", "Red Dragon");

	return TileNames;
}

bool AMahjongTile::IsNextTile(TEnumAsByte<ETileId> NextTileId, bool bExtendedSearch) const {
	return IsNextTile(TileId, NextTileId, bExtendedSearch);
}

bool AMahjongTile::IsNextTile(const AMahjongTile* NextTile, bool bExtendedSearch) const {
	return IsNextTile(TileId, NextTile->GetTileId(), bExtendedSearch);
}

void AMahjongTile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	DOREPLIFETIME(AMahjongTile, TileValue);
	DOREPLIFETIME(AMahjongTile, TileId);
	DOREPLIFETIME(AMahjongTile, bRedTile);
	DOREPLIFETIME(AMahjongTile, TileOwner);
	DOREPLIFETIME(AMahjongTile, PrevTileOwner);
}