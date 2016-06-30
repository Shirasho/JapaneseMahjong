// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "Online.h"
#include "MahjongGameSession.generated.h"

struct FMahjongGameSessionParams {

	/** The name of the session. */
	FName SessionName;
	/** Whether the match is a LAN match. */
	bool bIsLan;
	/** The Id of the player who initiated the lobby. */
	TSharedPtr<const FUniqueNetId> UserId;
	/** The current search result choice to join. */
	int32 SessionId;

	FMahjongGameSessionParams()
		: SessionName(NAME_None)
		, bIsLAN(false)
		, SessionId(0)
	{}
};

UCLASS(config=Game)
class MAHJONGGAME_API AMahjongGameSession : public AGameSession {

	GENERATED_BODY()

protected:

	AMahjongGameSession();

protected:

	/** Transient properties of a session during game creation/matchmaking */
	FShooterGameSessionParams CurrentSessionParams;
};