// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "Engine.h"				// Core Engine
#include "Net/UnrealNetwork.h"	// Replication
#include "MahjongTypes.h"		// Commonly-used types

// Log categories
DECLARE_LOG_CATEGORY_EXTERN(LogMahjong, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMahjongPlayer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMahjongServer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMahjongHand, Log, All);

// Player information
#define PLAYER_NAME_MAX_LENGTH	16

// Should we force the use of the console GUI?
#define MAHJONG_SIMULATE_CONSOLE_UI	0

// Are we using the console GUI or the desktop GUI?
#if PLATFORM_PS4 || PLATFORM_XBOXONE || MAHJONG_SIMULATE_CONSOLE_UI
	#define MAHJONG_CONSOLE_UI	1
#else
	#define MAHJONG_CONSOLE_UI	0
#endif