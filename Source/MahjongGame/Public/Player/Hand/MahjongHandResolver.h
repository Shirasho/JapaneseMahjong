// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "ETileId.h"

class UMahjongPlayerHand;
class AMahjongTile;

class FMahjongHandResolver {

protected:

	FMahjongHandResolver()
		: bTrackingUsedTiles(false)
	{
	}

public:

	// Begins tracking of tiles that were used to calculate
	// sets and sequences.
	void BeginResolve(const UMahjongPlayerHand* PlayerHand, bool bResetUsedTiles = true);

	// Ends tracking of tiles that were used to calculate
	// sets and sequences. Resets these tiles to be counted
	// again.
	void EndResolve();

	// Gets the Han value of the current hand.
	//virtual uint8 GetHanValue() const = 0;
	// Gets the Shanten value of the current hand.
	virtual uint8 GetShantenValue(const UMahjongPlayerHand* PlayerHand) = 0;

public:

	uint8 GetPartialSequenceCount(const UMahjongPlayerHand* PlayerHand);

	uint8 GetSequenceCount(uint8 SequenceLength, const UMahjongPlayerHand* PlayerHand);

	uint8 GetPairCount(uint8 PairLength, const UMahjongPlayerHand* PlayerHand);

private:

	void PopulateBuckets(const TArray<AMahjongTile*>& PlayerHand, TMap<uint8, TArray<const AMahjongTile*>>& OutTileBucket) const;

protected:

	void DecrementAvailableTiles(TEnumAsByte<ETileId> TileId, uint8 Count = 1);
	bool IsTileAvailable(TEnumAsByte<ETileId> TileId, uint8 Count = 1) const;
	void ResetAvailableTiles(const UMahjongPlayerHand* PlayerHand);
	
private:

	TMap<ETileId, uint8> RemainingTiles;
	bool bTrackingUsedTiles;
};