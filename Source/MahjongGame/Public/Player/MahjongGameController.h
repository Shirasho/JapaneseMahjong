// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once
#include "MahjongPlayerController.h"
#include "ETileId.h"
#include "MahjongGameController.generated.h"

class UMahjongPlayerHand;

UCLASS()
class AMahjongGameController : public AMahjongPlayerController
{
	GENERATED_BODY()

public:
	AMahjongGameController();

protected:

	UPROPERTY(BlueprintReadOnly, Category = Tiles)
	UMahjongPlayerHand* PlayerHand;

	UPROPERTY(BlueprintReadOnly, Category = Player)
		ETileId Wind;

public:

	void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = Player)
		FORCEINLINE ETileId GetWind() const {
		return Wind;
	}

	UFUNCTION(BlueprintCallable, Category = Player)
		void SetWind(ETileId InWind) {
		check(InWind == ETileId::WIND_NAN ||
			InWind == ETileId::WIND_PEI ||
			InWind == ETileId::WIND_SHA ||
			InWind == ETileId::WIND_TON);
		Wind = InWind;
	}
};


