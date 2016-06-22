// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongGameMode.h"
#include "MahjongMenuGameMode.generated.h"

UCLASS()
class JAPANESEMAHJONG_API AMahjongMenuGameMode : public AMahjongGameMode
{
	GENERATED_BODY()
	
protected:

	AMahjongMenuGameMode();

public:

	virtual void BeginPlay() override;
};