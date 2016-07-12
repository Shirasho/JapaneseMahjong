// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#ifndef __MAHJONGGAMELOADINGSCREEN_H__
#define __MAHJONGGAMELOADINGSCREEN_H__

#include "ModuleInterface.h"


/** Module interface for this game's loading screens */
class IMahjongGameLoadingScreenModule : public IModuleInterface
{
public:
	/** Kicks off the loading screen for in game loading (not startup) */
	virtual void StartInGameLoadingScreen() = 0;
};

#endif // __MAHJONGGAMELOADINGSCREEN_H__
