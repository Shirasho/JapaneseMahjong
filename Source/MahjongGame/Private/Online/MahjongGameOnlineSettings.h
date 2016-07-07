// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "Online.h"

// General settings for a Mahjong game.
class FMahjongOnlineSessionSettings : public FOnlineSessionSettings
{
public:

    FMahjongOnlineSessionSettings(bool bIsLAN = false, bool bIsPresence = false, int32 MaxNumPlayers = 4);
    virtual ~FMahjongOnlineSessionSettings() {}
};

// General search settings for a Mahjong game.
class FMahjongOnlineSearchSettings : public FOnlineSessionSearch
{
public:
    FMahjongOnlineSearchSettings(bool bSearchingLAN = false, bool bSearchingPresence = false);
    virtual ~FMahjongOnlineSearchSettings() {}
};

// Search settings for an empty dedicated server to host a match.
class FMahjongOnlineSearchSettingsEmptyDedicated : public FMahjongOnlineSearchSettings
{
public:
    FMahjongOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN = false, bool bSearchingPresence = false);
    virtual ~FMahjongOnlineSearchSettingsEmptyDedicated() {}
};