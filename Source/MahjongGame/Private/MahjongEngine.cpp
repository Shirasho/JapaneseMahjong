// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongEngine.h"

#include "MahjongGameInstance.h"

UMahjongEngine::UMahjongEngine() : Super() {

}

void UMahjongEngine::Init(IEngineLoop* InEngineLoop) {
	Super::Init(InEngineLoop);
}

void UMahjongEngine::HandleNetworkFailure(UWorld *World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	// Parent failure handling.
	Super::HandleNetworkFailure(World, NetDriver, FailureType, ErrorString);
}

