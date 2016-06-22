// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"
#include "JapaneseMahjongHandResolver.h"

#include "MahjongPlayerHand.h"
#include "MahjongTile.h"
#include "MahjongGameController.h"


uint8 FJapaneseMahjongHandResolver::GetShantenValue(const UMahjongPlayerHand* PlayerHand) {

	int8 ShantenValue = 8;

	BeginResolve(PlayerHand);
	ShantenValue -= (GetSequenceCount(3, PlayerHand) * 2) + (GetPairCount(3, PlayerHand) * 2) + (GetPairCount(2, PlayerHand)) + (GetPartialSequenceCount(PlayerHand));
	EndResolve();

	check(ShantenValue >= 0);

	return (uint8)ShantenValue;
}

bool FJapaneseMahjongHandResolver::IsTenpai(const UMahjongPlayerHand* PlayerHand) {
	return GetShantenValue(PlayerHand) == 0;
}

bool FJapaneseMahjongHandResolver::IsRiichi(const UMahjongPlayerHand* PlayerHand) {
	return !PlayerHand->IsHandOpen() && IsTenpai(PlayerHand);
}



bool FJapaneseMahjongHandResolver::IsTsumo(const UMahjongPlayerHand* PlayerHand) {
	return !PlayerHand->IsHandOpen() &&
		PlayerHand->GetDrawTile()->GetPrevTileOwner() == nullptr &&
		IsRiichi(PlayerHand);
}

bool FJapaneseMahjongHandResolver::IsRon(const UMahjongPlayerHand* PlayerHand) {
	return PlayerHand->IsHandOpen() &&
		IsRiichi(PlayerHand);
}




bool FJapaneseMahjongHandResolver::IsPinfu(const UMahjongPlayerHand* PlayerHand) {
	return GetPairCount(3, PlayerHand) == 0 && // 3 implies 4 since 3 behaves as a subset of 4.
		IsRiichi(PlayerHand) &&
		!PlayerHand->ContainsDragonTile() && // Can't contain a pair of dragons, so this implies not owning any.
		!PlayerHand->ContainsTile(PlayerHand->GetMahjongPlayer()->GetWind()) &&
		//!PlayerHand->ContainsTile(PlayerHand->GetMahjongPlayer()->GETSOMETHING()->GetRoundWind()) &&
		!PlayerHand->IsHandOpen() &&
		//IsOpenWait(PlayerHand) && // Waiting on two or more tiles for a single sequence.
		//!IsClosedWait(PlayerHand) // Waiting on a tile that completes the center of a sequence (i.e. 5-7, waiting on 6)
		//!IsEdgeWait(PlayerHand)   // Waiting SOLELY on a 3 or a 7 to complete a sequence (chow)
		//!IsPairWait(PlayerHand)   // Waiting on the tile that will complete the pair [5-5-5-1-2-3-8] -> waiting on the 8
		true;
}


bool FJapaneseMahjongHandResolver::IsIkkitsuukan(const UMahjongPlayerHand* PlayerHand) {
	
	bool Result = false;
	
	BeginResolve(PlayerHand);
	
	Result = GetSequenceCount(9, PlayerHand) &&
			(GetSequenceCount(3, PlayerHand) == 1 || GetPairCount(3, PlayerHand) == 1) &&
			IsRiichi(PlayerHand); // This line is overkill since the above implies 6 + 2 shanten for tenpai.
	
	EndResolve();

	return Result;
}

//http://saki.wikia.com/wiki/Mahjong_waits
//IsClosedWait(PlayerHand) // Waiting on a tile that completes the center of a sequence (i.e. 5-7, waiting on 6)
//IsEdgeWait(PlayerHand)   // Waiting SOLELY on a 3 or a 7 to complete a sequence (chow)
//IsOpenWait(PlayerHand)   // Waiting on two or more tiles for a sequence
//IsDualWait(PlayerHand)   // Waiting on two different tiles to complete one of two existing tile pairs [5-5-9-9-1-2-3] -> wait on 5 or 9
//IsPairWait(PlayerHand)   // Waiting on the tile that will complete the pair [5-5-5-1-2-3-8] -> waiting on the 8