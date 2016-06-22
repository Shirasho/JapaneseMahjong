// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

UENUM(BlueprintType)
enum class ETileId : uint8
{
	INVALID = 0,
	WAN_1,	// Characters, Numbers
	WAN_2,
	WAN_3,
	WAN_4,
	WAN_5,
	WAN_6,
	WAN_7,
	WAN_8,
	WAN_9,
	PIN_1,	// Circles
	PIN_2,
	PIN_3,
	PIN_4,
	PIN_5,
	PIN_6,
	PIN_7,
	PIN_8,
	PIN_9,
	SOU_1,	// Bamboo
	SOU_2,
	SOU_3,
	SOU_4,
	SOU_5,
	SOU_6,
	SOU_7,
	SOU_8,
	SOU_9,
	WIND_TON,	// East Wind
	WIND_NAN,	// South Wind
	WIND_SHA,	// West Wind
	WIND_PEI,	// North Wind
	DRAGON_HAKU,	// White Dragon
	DRAGON_HATSU,	// Green Dragon
	DRAGON_CHUN,	// Red Dragon

	TILE_ID_MAX
};