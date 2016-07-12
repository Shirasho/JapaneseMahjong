// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

UENUM(BlueprintType)
enum class EMahjongGameMode : uint8
{
	INVALID = 0,
	JAPANESE,
	SOLITAIRE
};

UENUM(BlueprintType)
enum class EMahjongTileId : uint8
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

enum class EMahjongPlayerWind : uint8 {
	WIND_NONE = 0,
	WIND_TON = EMahjongTileId::WIND_TON,	// East Wind
	WIND_NAN = EMahjongTileId::WIND_NAN,	// South Wind
	WIND_SHA = EMahjongTileId::WIND_SHA,	// West Wind
	WIND_PEI = EMahjongTileId::WIND_PEI	// North Wind
};

/*
* The UHT is incapable of the following:
*	- Unordered values (each value must be greater than the previous)
*	- Equal values (two enum values may not be the same)
*	- Bit operations (cannot assign '(1 << 0)' or '(2 | 4)' to a value)
*	- Back referencing (cannot use a previous enum value - ex. HONOR = WIND | DRAGON)
*
* This means that all UENUM enum values must be numeric, sorted constants.
*/

UENUM(BlueprintType)
enum class EMahjongTileKind : uint8
{
	NONE = 0x00,
	WAN = 0x01,
	PIN = 0x02,
	SOU = 0x04,
	SUITE = 0x07,
	WIND = 0x08,
	DRAGON = 0x10,
	HONOR = 0x18
};

namespace EMahjongDialogType
{
	enum Type
	{
		None,
		Generic,
		ControllerDisconnected
	};
}

namespace EMahjongGameState
{
    enum Type
    {
        Warmup,
        Playing,
        Won,
        Lost
    };
}

namespace MahjongWords
{
    const FString CustomMatchKeyword("Custom");
}