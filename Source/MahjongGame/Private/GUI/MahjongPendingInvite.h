// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

class FMahjongPendingInvite
{
public:
    FMahjongPendingInvite() : ControllerId(-1), UserId(nullptr), bPrivilegesCheckedAndAllowed(false) {}

    int32							 ControllerId;
    TSharedPtr< const FUniqueNetId > UserId;
    FOnlineSessionSearchResult 		 InviteResult;
    bool							 bPrivilegesCheckedAndAllowed;
};