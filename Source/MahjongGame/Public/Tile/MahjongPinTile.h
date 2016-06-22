// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "Tile/MahjongTile.h"
#include "MahjongPinTile.generated.h"

/**
 * 
 */
UCLASS()
class JAPANESEMAHJONG_API AMahjongPinTile : public AMahjongTile
{
	GENERATED_BODY()
	
protected:
	// Sets default values for this actor's properties
	AMahjongPinTile();

public:

	void Init(uint8 TileValue, bool bRedTile);

public:

	UFUNCTION(BlueprintPure, Category = Tile)
	FORCEINLINE ETileKind GetTileKind() const override {
		return ETileKind::PIN;
	}

	FORCEINLINE friend bool operator==(const AMahjongPinTile& This, const AMahjongPinTile& Other) {
		return (This.GetTileId() == Other.GetTileId());
	}

	FORCEINLINE friend bool operator!=(const AMahjongPinTile& This, const AMahjongPinTile& Other) {
		return (This.GetTileId() != Other.GetTileId());
	}

	FORCEINLINE friend bool operator <(const AMahjongPinTile& This, const AMahjongPinTile& Other) {
		return (This.GetTileId() < Other.GetTileId());
	}

	FORCEINLINE friend bool operator >(const AMahjongPinTile& This, const AMahjongPinTile& Other) {
		return (This.GetTileId() > Other.GetTileId());
	}
};
