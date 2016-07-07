// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongGameMode.h"
#include "MahjongGameMode_Solitaire.generated.h"

UCLASS()
class AMahjongGameMode_Solitaire : public AMahjongGameMode {

	GENERATED_UCLASS_BODY()

public:

    EMahjongGameMode GetGameMode() const override;
};