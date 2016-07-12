// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameSession_Japanese.h"

#include "Private/Online/MahjongGameOnlineSettings.h"


AMahjongGameSession_Japanese::AMahjongGameSession_Japanese(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

uint8 AMahjongGameSession_Japanese::GetMaxPlayers() const
{
    return 4;
}

bool AMahjongGameSession_Japanese::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FString& GameType, const FString& MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
    IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        CurrentSessionParams.SessionName = SessionName;
        CurrentSessionParams.bIsLAN = bIsLAN;
        CurrentSessionParams.bIsPresence = bIsPresence;
        CurrentSessionParams.UserId = UserId;
        MaxPlayers = MaxNumPlayers;

        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid() && CurrentSessionParams.UserId.IsValid())
        {
            HostSettings = MakeShareable(new FMahjongOnlineSessionSettings(bIsLAN, bIsPresence, MaxPlayers));
            HostSettings->Set(SETTING_GAMEMODE, GameType, EOnlineDataAdvertisementType::ViaOnlineService);
            HostSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
            HostSettings->Set(SETTING_MATCHING_HOPPER, FString("JapaneseMahjong"), EOnlineDataAdvertisementType::DontAdvertise);
            HostSettings->Set(SETTING_MATCHING_TIMEOUT, 120.0f, EOnlineDataAdvertisementType::ViaOnlineService);
            HostSettings->Set(SETTING_SESSION_TEMPLATE_NAME, FString("GameSession"), EOnlineDataAdvertisementType::DontAdvertise);
            HostSettings->Set(SEARCH_KEYWORDS, MahjongWords::CustomMatchKeyword, EOnlineDataAdvertisementType::ViaOnlineService);

            OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
            return Sessions->CreateSession(*CurrentSessionParams.UserId, CurrentSessionParams.SessionName, *HostSettings);
        }
    }
#if !UE_BUILD_SHIPPING
    else
    {
        // Hack workflow in development
        OnCreatePresenceSessionComplete().Broadcast(GameSessionName, true);
        return true;
    }
#endif

    return false;
}

uint8 AMahjongGameSession::GetMaxPlayers() const
{
    return 1;
}