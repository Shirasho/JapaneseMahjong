// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameMode_Solitaire.h"


AMahjongGameMode_Solitaire::AMahjongGameMode_Solitaire(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	// Whether the game should immediately start when the first player logs in. Affects the default behavior of ReadyToStartMatch.
	bDelayedStart = true;
}