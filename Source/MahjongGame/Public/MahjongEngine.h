// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongEngine.generated.h"

UCLASS()
class MAHJONGGAME_API UMahjongEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	UMahjongEngine();

private:

	virtual void Init(IEngineLoop* InEngineLoop);

public:

	virtual void HandleNetworkFailure(UWorld *World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) override;
};