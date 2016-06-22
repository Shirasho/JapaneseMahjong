// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongGameMode.h"
#include "MahjongGameGameMode.generated.h"

UCLASS()
class JAPANESEMAHJONG_API AMahjongGameGameMode : public AMahjongGameMode
{
	GENERATED_BODY()
	
protected:

	AMahjongGameGameMode();

public:

	virtual void BeginPlay() override;
};