// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"		// Required include
#include "MahjongHandResolver.h"	// Required include

#include "MahjongPlayerHand.h"
#include "MahjongTile.h"

void FMahjongHandResolver::DecrementAvailableTiles(TEnumAsByte<ETileId> TileId, uint8 Count) {
	if (bTrackingUsedTiles) {
		RemainingTiles.FindChecked(TileId) -= Count;
	}
}

bool FMahjongHandResolver::IsTileAvailable(TEnumAsByte<ETileId> TileId, uint8 Count) const {
	if (bTrackingUsedTiles) {
		const uint8* TileCount = RemainingTiles.Find(TileId);
		return TileCount && *TileCount >= Count;
	}
	return true;
}

void FMahjongHandResolver::ResetAvailableTiles(const UMahjongPlayerHand* PlayerHand) {

	const TArray<AMahjongTile*> HandTiles = PlayerHand->GetHand();
	RemainingTiles.Empty(HandTiles.Num());

	for (const AMahjongTile* Tile : HandTiles) {
		++RemainingTiles.FindOrAdd(Tile->GetTileId());
	}
}

void FMahjongHandResolver::BeginResolve(const UMahjongPlayerHand* PlayerHand, bool bResetUsedTiles) {
	bTrackingUsedTiles = true;
	if (bResetUsedTiles) {
		ResetAvailableTiles(PlayerHand);
	}
}
void FMahjongHandResolver::EndResolve() {
	bTrackingUsedTiles = false;
}

void FMahjongHandResolver::PopulateBuckets(const TArray<AMahjongTile*>& PlayerHand, TMap<uint8, TArray<const AMahjongTile*>>& OutTileBucket) const {
	for (const AMahjongTile* Tile : PlayerHand) {
		OutTileBucket.FindOrAdd((uint8)Tile->GetTileId()).Add(Tile);
	}
}

uint8 FMahjongHandResolver::GetPartialSequenceCount(const UMahjongPlayerHand* PlayerHand) {

	check(PlayerHand);

	TArray<AMahjongTile*> Tiles = PlayerHand->GetHand();
	TMap<uint8, TArray<const AMahjongTile*>> TileBucket;

	// Populates TileBucket by reference using Tiles.
	PopulateBuckets(Tiles, TileBucket);

	// The number of total sequences of the desired length found.
	uint8 Sequences = 0;
	// The Id of the first tile in the sequence we are checking.
	uint8 CurrentId = static_cast<uint8>(ETileId::WAN_1);

	// For all possible tile Ids (Winds will be implemented separately if wind straights are actually a thing)
	while (CurrentId <= static_cast<uint8>(ETileId::SOU_9)) {

		TArray<const AMahjongTile*>* CurrentBucket;

		// While the current tile bucket still has a tile in it.
		while ((CurrentBucket = TileBucket.Find(CurrentId)) != nullptr && CurrentBucket->Num() > 0 && IsTileAvailable(TEnumAsByte<ETileId>(CurrentId))) {

			if (!IsTileAvailable(TEnumAsByte<ETileId>(CurrentId))) {
				++CurrentId;
				break;
			}

			// Seek the next and subsequent tiles buckets.
			TArray<const AMahjongTile*>* NextBucket;
			
			// For the next tile Id and the Id after it (i.e. 3-4 or 6-8)
			// If the bucket exists, has uncounted tiles left, has persistent tracked tiles left, and the next bucket is actually a valid "next tile"
			if (((NextBucket = TileBucket.Find(CurrentId + 1)) != nullptr && NextBucket->Num() > 0 && IsTileAvailable(TEnumAsByte<ETileId>(CurrentId+1)) && AMahjongTile::IsNextTile(TEnumAsByte<ETileId>(CurrentId), TEnumAsByte<ETileId>(CurrentId + 1), false)) ||
				((NextBucket = TileBucket.Find(CurrentId + 2)) != nullptr && NextBucket->Num() > 0 && IsTileAvailable(TEnumAsByte<ETileId>(CurrentId+2)) && AMahjongTile::IsNextTile(TEnumAsByte<ETileId>(CurrentId), TEnumAsByte<ETileId>(CurrentId + 2), true))) {

				// Pop one tile from the associated buckets.
				const AMahjongTile* CurrentTile = CurrentBucket->Pop(false);
				const AMahjongTile* NextTile = NextBucket->Pop(false);

				// Decrement the persist tiles if resolving.
				DecrementAvailableTiles(TEnumAsByte<ETileId>(CurrentTile->GetTileId()));
				DecrementAvailableTiles(TEnumAsByte<ETileId>(NextTile->GetTileId()));

				++Sequences;
			}
			else {
				++CurrentId;
			}
		}

		++CurrentId;
	}

	return Sequences;
}

uint8 FMahjongHandResolver::GetSequenceCount(uint8 SequenceLength, const UMahjongPlayerHand* PlayerHand) {

	check(PlayerHand);

	TArray<AMahjongTile*> Tiles = PlayerHand->GetHand();
	TMap<uint8, TArray<const AMahjongTile*>> TileBucket;

	// Populates TileBucket by reference using Tiles.
	PopulateBuckets(Tiles, TileBucket);

	// The number of total sequences of the desired length found.
	uint8 Sequences = 0;
	// The Id of the first tile in the sequence we are checking.
	uint8 CurrentStartId = static_cast<uint8>(ETileId::WAN_1);
	// The incremented Id of the subsequent tiles in the sequence we are checking.
	uint8 CurrentId = CurrentStartId;
	// The length of the currently checked sequence.
	uint8 SeqLen = 0;

	// For all possible tile Ids (Winds will be implemented separately if wind straights are actually a thing)
	while (CurrentStartId <= static_cast<uint8>(ETileId::SOU_9)) {

		// Set the tile index seeker to the first tile Id.
		CurrentId = CurrentStartId;
		// Reset our seq length tracker.
		SeqLen = 0;

		TArray<const AMahjongTile*>* CurrentStartBucket;

		// While the first tile bucket still has a tile in it.
		while ((CurrentStartBucket = TileBucket.Find(CurrentStartId)) != nullptr && CurrentStartBucket->Num() > 0 && IsTileAvailable(TEnumAsByte<ETileId>(CurrentStartId))) {

			// Reset our seq length tracker.
			SeqLen = 0;

			TArray<const AMahjongTile*>* CurrentBucket;
			// While subsequent buckets exist, aren't empty, and the sequence chain is less than the desired length
			while ((CurrentBucket = TileBucket.Find(CurrentId)) != nullptr && CurrentBucket->Num() > 0 && IsTileAvailable(TEnumAsByte<ETileId>(CurrentId)) && SeqLen < SequenceLength) {

				++CurrentId;
				++SeqLen;

				// If the next item to check isn't the next tile of the one we just checked, break.
				if (!AMahjongTile::IsNextTile(TEnumAsByte<ETileId>(CurrentId-1), TEnumAsByte<ETileId>(CurrentId), false)) {
					break;
				}
			}

			// If the sequence length is the desired length, remove one tile
			// from each bucket, adding it to the counted tiles if resolving.
			if (SeqLen == SequenceLength) {
				for (uint8 TileIndex = CurrentStartId; TileIndex < CurrentStartId + SequenceLength; ++TileIndex) {

					TArray<const AMahjongTile*>* Tiles = TileBucket.Find(TileIndex);
					const AMahjongTile* CountedTile = Tiles->Pop(false);
					DecrementAvailableTiles(TEnumAsByte<ETileId>(CountedTile->GetTileId()));
				}
				// Increment our sequence count.
				++Sequences;
				// Reset our current Id to the start Id in case we have two of the same tile.
				CurrentId = CurrentStartId;
			} else {
				// If the bucket search was broken due to an empty bucket we should
				// go to that bucket since any sequences before it will be terminated
				// by that bucket. Move on.
				++CurrentStartId;
			}
		}

		++CurrentStartId;
	}

	return Sequences;
}

uint8 FMahjongHandResolver::GetPairCount(uint8 PairLength, const UMahjongPlayerHand* PlayerHand) {

	check(PlayerHand);

	TArray<AMahjongTile*> Tiles = PlayerHand->GetHand();
	TMap<uint8, TArray<const AMahjongTile*>> TileBucket;

	// Populates TileBucket by reference using Tiles.
	PopulateBuckets(Tiles, TileBucket);

	// The number of total pairs of the desired length found.
	uint8 Pairs = 0;

	for (auto& Tiles : TileBucket) {
		while (Tiles.Value.Num() >= PairLength && IsTileAvailable(TEnumAsByte<ETileId>(Tiles.Key), PairLength)) {
			Tiles.Value.RemoveAt(0, PairLength, false);
			DecrementAvailableTiles(TEnumAsByte<ETileId>(Tiles.Key), PairLength);
			++Pairs;
		}
	}

	return Pairs;
}