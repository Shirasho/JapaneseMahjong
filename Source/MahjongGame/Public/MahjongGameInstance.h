// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGameInstance.generated.h"

class AMahjongGameSession;

namespace MahjongGameInstanceState
{
	extern const FName None;
	extern const FName PendingInvite;
	extern const FName WelcomeScreen;
	extern const FName MainMenu;
	extern const FName MessageMenu;
	extern const FName Playing;
}

UCLASS(config=Game)
class UMahjongGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UMahjongGameInstance();

private:

	/** The current game state. */
	FName CurrentState;

	/** The pending game state. */
	FName PendingState;

	/** Whether the match is online or not */
	bool bIsOnline;

};