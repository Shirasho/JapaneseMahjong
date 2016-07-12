// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

// Should we force the use of the console GUI?
#define MAHJONG_GUI_SIMULATE_CONSOLE_UI	0

// Are we using the console GUI or the desktop GUI?
#if PLATFORM_PS4 || PLATFORM_XBOXONE || MAHJONG_GUI_SIMULATE_CONSOLE_UI
#define MAHJONG_GUI_CONSOLE_UI	1
#else
#define MAHJONG_GUI_CONSOLE_UI	0
#endif