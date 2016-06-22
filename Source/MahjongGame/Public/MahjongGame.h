// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "Engine.h"
#include "MahjongTypes.h"

// Log categories
DECLARE_LOG_CATEGORY_EXTERN(LogMahjong, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMahjongPlayerAction, Log, All);

// Player information
#define PLAYER_NAME_MAX_LENGTH	16

// Debugging
#define MAHJONG_SIMULATE_CONSOLE_UI	0

// GUI
#if PLATFORM_PS4 || PLATFORM_XBOXONE || MAHJONG_SIMULATE_CONSOLE_UI
	#define MAHJONG_CONSOLE_UI	1
#else
	#define MAHJONG_CONSOLE_UI	0
#endif