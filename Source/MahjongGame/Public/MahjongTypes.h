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

UENUM(BlueprintType)
enum class EMahjongPlayerWind : uint8 {
	WIND_NONE = 0,
	WIND_TON,	// East Wind
	WIND_NAN,	// South Wind
	WIND_SHA,	// West Wind
	WIND_PEI	// North Wind
};

// Since the player wind needs to be set in places it wouldn't be appropriate to request
// a EMahjongTileId value since a bunch of invalid values can be passed with it. This method
// is used to determine whether a PlayerWind value is the same as the TileId value for the
// corresponding wind value. Example usage would be checking if a tile is the same as
// the current player's wind (which is used to determine a valid hand in some cases).
static bool MahjongAreWindsEqual(EMahjongTileId TileId, EMahjongPlayerWind WindId)
{
    return (TileId == EMahjongTileId::WIND_TON && WindId == EMahjongPlayerWind::WIND_TON) ||
        (TileId == EMahjongTileId::WIND_NAN && WindId == EMahjongPlayerWind::WIND_NAN) ||
        (TileId == EMahjongTileId::WIND_SHA && WindId == EMahjongPlayerWind::WIND_SHA) ||
        (TileId == EMahjongTileId::WIND_PEI && WindId == EMahjongPlayerWind::WIND_PEI);
}
// Since the player wind needs to be set in places it wouldn't be appropriate to request
// a EMahjongTileId value since a bunch of invalid values can be passed with it. This method
// is used to determine whether a PlayerWind value is the same as the TileId value for the
// corresponding wind value. Example usage would be checking if a tile is the same as
// the current player's wind (which is used to determine a valid hand in some cases).
static bool MahjongAreWindsEqual(EMahjongPlayerWind WindId, EMahjongTileId TileId)
{
    return MahjongAreWindsEqual(TileId, WindId);
}
// Since the player wind needs to be set in places it wouldn't be appropriate to request
// a EMahjongTileId value since a bunch of invalid values can be passed with it. This method
// is used to determine whether a PlayerWind value is the same as the TileId value for the
// corresponding wind value. Example usage would be checking if a tile is the same as
// the current player's wind (which is used to determine a valid hand in some cases).
static bool MahjongAreWindsEqual(uint8 TileId, EMahjongPlayerWind WindId)
{
    return MahjongAreWindsEqual(static_cast<EMahjongTileId>(TileId), WindId);
}
// Since the player wind needs to be set in places it wouldn't be appropriate to request
// a EMahjongTileId value since a bunch of invalid values can be passed with it. This method
// is used to determine whether a PlayerWind value is the same as the TileId value for the
// corresponding wind value. Example usage would be checking if a tile is the same as
// the current player's wind (which is used to determine a valid hand in some cases).
static bool MahjongAreWindsEqual(EMahjongTileId TileId, uint8 WindId)
{
    return MahjongAreWindsEqual(TileId, static_cast<EMahjongPlayerWind>(WindId));
}
// Since the player wind needs to be set in places it wouldn't be appropriate to request
// a EMahjongTileId value since a bunch of invalid values can be passed with it. This method
// is used to determine whether a PlayerWind value is the same as the TileId value for the
// corresponding wind value. Example usage would be checking if a tile is the same as
// the current player's wind (which is used to determine a valid hand in some cases).
static bool MahjongAreWindsEqual(uint8 TileId, uint8 WindId)
{
    return MahjongAreWindsEqual(static_cast<EMahjongTileId>(TileId), static_cast<EMahjongPlayerWind>(WindId));
}

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