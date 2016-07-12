// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "Engine.h"				// Core Engine
#include "Online.h"             // Online
#include "Net/UnrealNetwork.h"	// Replication
#include "MahjongTypes.h"		// Commonly-used types
#include "ParticleDefinitions.h"// Particle systems.
#include "SoundDefinitions.h"   // Sound systems.
#include "PlatformDefinitions.h"// Platform definitions.

// Log categories
DECLARE_LOG_CATEGORY_EXTERN(LogMahjong, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMahjongPlayer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMahjongServer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMahjongHand, Log, All);