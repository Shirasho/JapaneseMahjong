// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongGameSession.h"
#include "MahjongGameSession_Japanese.generated.h"

UCLASS(config = Game)
class MAHJONGGAME_API AMahjongGameSession_Japanese : public AMahjongGameSession
{

    GENERATED_UCLASS_BODY()

public:

    virtual uint8 GetMaxPlayers() const override;

    /**
    * Host a new online session
    *
    * @param UserId user that initiated the request
    * @param SessionName name of session
    * @param bIsLAN is this going to hosted over LAN
    * @param bIsPresence is the session to create a presence session
    * @param MaxNumPlayers Maximum number of players to allow in the session
    *
    * @return bool true if successful, false otherwise
    */
    virtual bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FString& GameType, const FString& MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers) override;

};