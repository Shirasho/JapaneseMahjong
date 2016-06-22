// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

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
enum class ETileKind : uint8
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