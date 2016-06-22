// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongHandResolver.h"

class FJapaneseMahjongHandResolver : public FMahjongHandResolver {

public:

	FJapaneseMahjongHandResolver() : FMahjongHandResolver() {

	}

	uint8 GetShantenValue(const UMahjongPlayerHand* PlayerHand) override;

	#pragma region Han Modifiers
		// Is the hand one tile away from winning?
		bool IsRiichi(const UMahjongPlayerHand* PlayerHand);
		bool IsTenpai(const UMahjongPlayerHand* PlayerHand);
		// Is the hand one tile away from winning on the first turn?
	//	bool IsDoubleRiichi(const UMahjongPlayerHand* PlayerHand);
		// Is this hand a winner due to self-draw?
		bool IsTsumo(const UMahjongPlayerHand* PlayerHand);
		// Is this hand a winner?
		bool IsRon(const UMahjongPlayerHand* PlayerHand);
		// Did the player win immediately after calling Riichi?
	//	bool IsIppatsu(const UMahjongPlayerHand* PlayerHand);
		// Did the player win by drawing the last tile from the wall?
	//	bool IsHaitei(const UMahjongPlayerHand* PlayerHand);
		// Did the player win due to the last discard by the last player that drew last from the wall?
	//	bool IsHoutei(const UMahjongPlayerHand* PlayerHand);
		// Did the player win due a draw from the dead wall due to Kan?
	//	bool IsRinshanKaihou(const UMahjongPlayerHand* PlayerHand);
		// Did the player win due to stealing the 4th tile from a Kan?
	//	bool IsChankan(const UMahjongPlayerHand* PlayerHand);
	#pragma endregion
	
	//https://en.wikipedia.org/wiki/Japanese_Mahjong_yaku
	
	#pragma region Sequence Yaku
		// Did the player win due to a hand with no triplets or Kan, no dragons, the player's own wind, or the round wind?
		bool IsPinfu(const UMahjongPlayerHand* PlayerHand);
		// Did the player win due to a hand with two identical sequences of the same suit?
	//	bool IsIipeikou(const UMahjongPlayerHand* PlayerHand);
		// Did the player win with three sequences in all three suits?
	//	bool IsSanshoku(const UMahjongPlayerHand* PlayerHand);
		// Did the player win with a straight 1-9 in the same suit?
		bool IsIkkitsuukan(const UMahjongPlayerHand* PlayerHand);
		// Did the player win with two sets of identical sequences, and a third sequence equal to the first two? For example, 1-3pin, 1-3pin, 1-3wan
	//	bool IsRyanpeikou(const UMahjongPlayerHand* PlayerHand);
	#pragma endregion
	
	#pragma region Pair Yaku
		// Did the player win with all triplets?
	//	bool IsToitoi(const UMahjongPlayerHand* PlayerHand);
	#pragma endregion
	
		// Is this hand a winner due to 7 pairs?
	//	bool IsChiitoitsu(const UMahjongPlayerHand* PlayerHand);
	
	#pragma endregion
};