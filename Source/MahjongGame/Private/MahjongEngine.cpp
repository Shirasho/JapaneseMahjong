// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongEngine.h"

#include "MahjongGameInstance.h"

UMahjongEngine::UMahjongEngine()
	: Super()
{

}

void UMahjongEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}

void UMahjongEngine::HandleNetworkFailure(UWorld *World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    // Only handle failure at this level for game or pending net drivers.
    FName NetDriverName = NetDriver ? NetDriver->NetDriverName : NAME_None;
    if (NetDriverName == NAME_GameNetDriver || NetDriverName == NAME_PendingNetDriver)
    {
        // If this net driver has already been unregistered with this world, then don't handle it.
        if (World && NetDriver)
        {
            switch (FailureType)
            {
                case ENetworkFailure::FailureReceived:
                {
                    UMahjongGameInstance* const MahjongInstance = Cast<UMahjongGameInstance>(GameInstance);
                    if (MahjongInstance && NetDriver->GetNetMode() == NM_Client)
                    {
                        const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

                        // NOTE - We pass in false here to not override the message if we are already going to the main menu
                        // We're going to make the assumption that someone else has a better message than "Lost connection to host" if
                        // this is the case
                        MahjongInstance->ShowMessageThenGotoState(FText::FromString(ErrorString), OKButton, FText::GetEmpty(), MahjongGameInstanceState::MainMenu, false);
                    }
                    break;
                }
                case ENetworkFailure::PendingConnectionFailure:
                {
                    UMahjongGameInstance* const GI = Cast<UMahjongGameInstance>(GameInstance);
                    if (GI && NetDriver->GetNetMode() == NM_Client)
                    {
                        const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

                        // NOTE - We pass in false here to not override the message if we are already going to the main menu
                        // We're going to make the assumption that someone else has a better message than "Lost connection to host" if
                        // this is the case
                        GI->ShowMessageThenGotoState(FText::FromString(ErrorString), OKButton, FText::GetEmpty(), MahjongGameInstanceState::MainMenu, false);
                    }
                    break;
                }
                case ENetworkFailure::ConnectionLost:
                case ENetworkFailure::ConnectionTimeout:
                {
                    UMahjongGameInstance* const GI = Cast<UMahjongGameInstance>(GameInstance);
                    if (GI && NetDriver->GetNetMode() == NM_Client)
                    {
                        const FText ReturnReason = NSLOCTEXT("NetworkErrors", "HostDisconnect", "Lost connection to host.");
                        const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

                        // NOTE - We pass in false here to not override the message if we are already going to the main menu
                        // We're going to make the assumption that someone else has a better message than "Lost connection to host" if
                        // this is the case
                        GI->ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), MahjongGameInstanceState::MainMenu, false);
                    }
                    break;
                }
                case ENetworkFailure::NetDriverAlreadyExists:
                case ENetworkFailure::NetDriverCreateFailure:
                case ENetworkFailure::OutdatedClient:
                case ENetworkFailure::OutdatedServer:
                default:
                    break;
            }
        }
    }

	// Parent failure handling.
	Super::HandleNetworkFailure(World, NetDriver, FailureType, ErrorString);
}

