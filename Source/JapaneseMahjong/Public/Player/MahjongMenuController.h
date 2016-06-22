// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once
#include "MahjongPlayerController.h"
#include "MahjongMenuController.generated.h"

UCLASS()
class AMahjongMenuController : public AMahjongPlayerController
{
	GENERATED_BODY()

public:
	AMahjongMenuController();

public:

	void BeginPlay() override;
};


