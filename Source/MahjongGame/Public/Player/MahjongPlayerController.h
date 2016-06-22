// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once
#include "GameFramework/PlayerController.h"
#include "MahjongPlayerController.generated.h"

UCLASS()
class AMahjongPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMahjongPlayerController();

public:

	void BeginPlay() override;
};


