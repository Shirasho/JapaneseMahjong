// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameOnlineSettings.h"


FMahjongOnlineSessionSettings::FMahjongOnlineSessionSettings(bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
    NumPublicConnections = MaxNumPlayers;
    if (NumPublicConnections < 0)
    {
        NumPublicConnections = 0;
    }
    NumPrivateConnections = 0;
    bIsLANMatch = bIsLAN;
    bShouldAdvertise = true;
    bAllowJoinInProgress = false;
    bAllowInvites = true;
    bUsesPresence = bIsPresence;
    bAllowJoinViaPresence = true;
    bAllowJoinViaPresenceFriendsOnly = false;
}

FMahjongOnlineSearchSettings::FMahjongOnlineSearchSettings(bool bSearchingLAN, bool bSearchingPresence)
{
    bIsLanQuery = bSearchingLAN;
    MaxSearchResults = 10;
    PingBucketSize = 50;

    if (bSearchingPresence)
    {
        QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
    }
}

FMahjongOnlineSearchSettingsEmptyDedicated::FMahjongOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN, bool bSearchingPresence) :
    FMahjongOnlineSearchSettings(bSearchingLAN, bSearchingPresence)
{
    QuerySettings.Set(SEARCH_DEDICATED_ONLY, true, EOnlineComparisonOp::Equals);
    QuerySettings.Set(SEARCH_EMPTY_SERVERS_ONLY, true, EOnlineComparisonOp::Equals);
}