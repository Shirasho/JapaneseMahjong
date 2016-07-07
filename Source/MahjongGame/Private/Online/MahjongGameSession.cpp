// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameSession.h"

#include "MahjongGameOnlineSettings.h"

#include "MahjongPlayerController.h"

namespace
{
    const FString CustomMatchKeyword("Custom");
}

AMahjongGameSession::AMahjongGameSession(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Only bind if this is not the default class object used as the template.
    if (!HasAnyFlags(RF_ClassDefaultObject))
    {
        OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &AMahjongGameSession::OnCreateSessionComplete);
        OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &AMahjongGameSession::OnDestroySessionComplete);

        OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &AMahjongGameSession::OnFindSessionsComplete);
        OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &AMahjongGameSession::OnJoinSessionComplete);

        OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &AMahjongGameSession::OnStartOnlineGameComplete);
    }
}

void AMahjongGameSession::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid())
        {
            Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
        }
    }

    if (bWasSuccessful)
    {
        // Tell all remote players to start an online game.
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            AMahjongPlayerController* PlayerController = Cast<AMahjongPlayerController>(*It);
            if (PlayerController && !PlayerController->IsLocalPlayerController())
            {
                PlayerController->ClientStartOnlineGame();
            }
        }
    }
}

void AMahjongGameSession::HandleMatchHasStarted()
{
    // Start the online game locally and wait for completion.
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid())
        {
            UE_LOG(LogOnlineGame, Log, TEXT("Starting session %s on server."), *GameSessionName.ToString());
            OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
            Sessions->StartSession(GameSessionName);
        }
    }
}

void AMahjongGameSession::HandleMatchHasEnded()
{
    // End the online game locally and wait for completion.
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid())
        {
            for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
            {
                AMahjongPlayerController* PlayerController = Cast<AMahjongPlayerController>(*It);
                if (PlayerController && !PlayerController->IsLocalPlayerController())
                {
                    PlayerController->ClientEndOnlineGame();
                }
            }

            UE_LOG(LogOnlineGame, Log, TEXT("Ending session %s on server."), *GameSessionName.ToString());
            Sessions->EndSession(GameSessionName);
        }
    }
}

bool AMahjongGameSession::IsBusy() const
{
    if (HostSettings.IsValid() || SearchSettings.IsValid())
    {
        return true;
    }

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid())
        {
            EOnlineSessionState::Type GameSessionState = Sessions->GetSessionState(GameSessionName);
            EOnlineSessionState::Type PartySessionState = Sessions->GetSessionState(PartySessionName);
            if (GameSessionState != EOnlineSessionState::NoSession || PartySessionState != EOnlineSessionState::NoSession)
            {
                return true;
            }
        }
    }

    return false;
}

EOnlineAsyncTaskState::Type AMahjongGameSession::GetSearchResultStatus(int32& SearchResultIdx, int32& NumSearchResults)
{
    SearchResultIdx = 0;
    NumSearchResults = 0;

    if (SearchSettings.IsValid())
    {
        if (SearchSettings->SearchState == EOnlineAsyncTaskState::Done)
        {
            SearchResultIdx = CurrentSessionParams.SessionId;
            NumSearchResults = SearchSettings->SearchResults.Num();
        }
        return SearchSettings->SearchState;
    }

    return EOnlineAsyncTaskState::NotStarted;
}

const TArray<FOnlineSessionSearchResult>& AMahjongGameSession::GetSearchResults() const
{
    return SearchSettings->SearchResults;
}

void AMahjongGameSession::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogOnlineGame, Verbose, TEXT("OnCreateSessionComplete %s bSuccess: %d"), *SessionName.ToString(), bWasSuccessful);

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid())
        {
            Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
        }
    }

    OnCreatePresenceSessionComplete().Broadcast(SessionName, bWasSuccessful);
}

void AMahjongGameSession::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogOnlineGame, Verbose, TEXT("OnDestroySessionComplete %s bSuccess: %d"), *SessionName.ToString(), bWasSuccessful);

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid())
        {
            Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
        }
        HostSettings = nullptr;
    }
}

bool AMahjongGameSession::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FString& GameType, const FString& MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
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
            HostSettings->Set(SETTING_MATCHING_HOPPER, FString("InvalidGameMode"), EOnlineDataAdvertisementType::DontAdvertise);
            HostSettings->Set(SETTING_MATCHING_TIMEOUT, 120.0f, EOnlineDataAdvertisementType::ViaOnlineService);
            HostSettings->Set(SETTING_SESSION_TEMPLATE_NAME, FString("GameSession"), EOnlineDataAdvertisementType::DontAdvertise);
            HostSettings->Set(SEARCH_KEYWORDS, CustomMatchKeyword, EOnlineDataAdvertisementType::ViaOnlineService);

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

void AMahjongGameSession::OnFindSessionsComplete(bool bWasSuccessful)
{
    UE_LOG(LogOnlineGame, Verbose, TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful);

    IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid())
        {
            Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

            UE_LOG(LogOnlineGame, Verbose, TEXT("Num Search Results: %d"), SearchSettings->SearchResults.Num());
            for (int32 SearchIdx = 0; SearchIdx < SearchSettings->SearchResults.Num(); SearchIdx++)
            {
                const FOnlineSessionSearchResult& SearchResult = SearchSettings->SearchResults[SearchIdx];
                DumpSession(&SearchResult.Session);
            }

            OnFindSessionsComplete().Broadcast(bWasSuccessful);
        }
    }
}

void AMahjongGameSession::ResetBestSessionVars()
{
    CurrentSessionParams.SessionId = -1;
}

void AMahjongGameSession::ChooseBestSession()
{
    // Start from where we left off.
    for (int32 SessionIndex = CurrentSessionParams.SessionId + 1; SessionIndex < SearchSettings->SearchResults.Num(); SessionIndex++)
    {
        // Found the match that we want
        CurrentSessionParams.SessionId = SessionIndex;
        return;
    }

    CurrentSessionParams.SessionId = -1;
}

void AMahjongGameSession::StartMatchmaking()
{
    ResetBestSessionVars();
    ContinueMatchmaking();
}

void AMahjongGameSession::ContinueMatchmaking()
{
    ChooseBestSession();
    if (CurrentSessionParams.SessionId >= 0 && CurrentSessionParams.SessionId < SearchSettings->SearchResults.Num())
    {
        IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
        if (OnlineSubsystem)
        {
            IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
            if (Sessions.IsValid() && CurrentSessionParams.UserId.IsValid())
            {
                OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
                Sessions->JoinSession(*CurrentSessionParams.UserId, CurrentSessionParams.SessionName, SearchSettings->SearchResults[CurrentSessionParams.SessionId]);
            }
        }
    }
    else
    {
        OnNoMatchesAvailable();
    }
}

void AMahjongGameSession::OnNoMatchesAvailable()
{
    UE_LOG(LogOnlineGame, Verbose, TEXT("Matchmaking complete, no sessions available."));
    SearchSettings = nullptr;
}

void AMahjongGameSession::FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence)
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        CurrentSessionParams.SessionName = SessionName;
        CurrentSessionParams.bIsLAN = bIsLAN;
        CurrentSessionParams.bIsPresence = bIsPresence;
        CurrentSessionParams.UserId = UserId;

        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid() && CurrentSessionParams.UserId.IsValid())
        {
            SearchSettings = MakeShareable(new FMahjongOnlineSearchSettings(bIsLAN, bIsPresence));
            SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, CustomMatchKeyword, EOnlineComparisonOp::Equals);

            TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SearchSettings.ToSharedRef();

            OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
            Sessions->FindSessions(*CurrentSessionParams.UserId, SearchSettingsRef);
        }
    }
    else
    {
        OnFindSessionsComplete(false);
    }
}

bool AMahjongGameSession::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, int32 SessionIndexInSearchResults)
{
    bool bResult = false;

    if (SessionIndexInSearchResults >= 0 && SessionIndexInSearchResults < SearchSettings->SearchResults.Num())
    {
        bResult = JoinSession(UserId, SessionName, SearchSettings->SearchResults[SessionIndexInSearchResults]);
    }

    return bResult;
}

bool AMahjongGameSession::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
    bool bResult = false;

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid() && UserId.IsValid())
        {
            OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
            bResult = Sessions->JoinSession(*UserId, SessionName, SearchResult);
        }
    }

    return bResult;
}

void AMahjongGameSession::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogOnlineGame, Verbose, TEXT("OnJoinSessionComplete %s bSuccess: %d"), *SessionName.ToString(), static_cast<int32>(Result));

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    IOnlineSessionPtr Sessions = nullptr;
    if (OnlineSubsystem)
    {
        Sessions = OnlineSubsystem->GetSessionInterface();
        Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
    }

    OnJoinSessionComplete().Broadcast(Result);
}

bool AMahjongGameSession::TravelToSession(int32 ControllerId, FName SessionName)
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        FString URL;
        IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid() && Sessions->GetResolvedConnectString(SessionName, URL))
        {
            APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), ControllerId);
            if (PlayerController)
            {
                PlayerController->ClientTravel(URL, TRAVEL_Absolute);
                return true;
            }
        }
        else
        {
            UE_LOG(LogOnlineGame, Warning, TEXT("Failed to join session %s"), *SessionName.ToString());
        }
    }
#if !UE_BUILD_SHIPPING
    else
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), ControllerId);
        if (PlayerController)
        {
            FString LocalURL(TEXT("127.0.0.1"));
            PlayerController->ClientTravel(LocalURL, TRAVEL_Absolute);
            return true;
        }
    }
#endif //!UE_BUILD_SHIPPING

    return false;
}