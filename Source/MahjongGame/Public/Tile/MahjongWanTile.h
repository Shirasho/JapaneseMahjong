// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "Tile/MahjongTile.h"
#include "MahjongWanTile.generated.h"

/**
 * 
 */
UCLASS()
class JAPANESEMAHJONG_API AMahjongWanTile : public AMahjongTile
{
	GENERATED_BODY()
	
protected:
	// Sets default values for this actor's properties
	AMahjongWanTile();

public:

	void Init(uint8 TileValue, bool bRedTile);

public:

	UFUNCTION(BlueprintPure, Category = Tile)
	FORCEINLINE ETileKind GetTileKind() const override {
		return ETileKind::WAN;
	}

	FORCEINLINE friend bool operator==(const AMahjongWanTile& This, const AMahjongWanTile& Other) {
		return (This.GetTileId() == Other.GetTileId());
	}

	FORCEINLINE friend bool operator!=(const AMahjongWanTile& This, const AMahjongWanTile& Other) {
		return (This.GetTileId() != Other.GetTileId());
	}

	FORCEINLINE friend bool operator <(const AMahjongWanTile& This, const AMahjongWanTile& Other) {
		return (This.GetTileId() < Other.GetTileId());
	}

	FORCEINLINE friend bool operator >(const AMahjongWanTile& This, const AMahjongWanTile& Other) {
		return (This.GetTileId() > Other.GetTileId());
	}
};