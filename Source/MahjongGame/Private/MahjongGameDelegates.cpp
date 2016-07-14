// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "GameDelegates.h"
#include "MahjongGameDelegates.h"

#include "MahjongMaps.h"
#include "MahjongGameState.h"

static void MahjongWebServerDelegate(int32 UserIndex, const FString& Action, const FString& URL, const TMap<FString, FString>& Params, TMap<FString, FString>& Response)
{
    if (URL == TEXT("/index.html?scoreboard"))
    {
        // Scoreboard JSON object.
        FString ScoreboardString = TEXT("{ \"scoreboard\" : [ ");

        // You shouldn't normally use this method to get a UWorld as it won't always be correct in a PIE context.
        // However, the PS4 companion app server will never run in the Editor.
        UGameEngine* const GameEngine = CastChecked<UGameEngine>(GEngine);
        if (GameEngine && !GEngine->IsEditor())
        {
            UWorld* const World = GameEngine->GetGameWorld();
            if (World)
            {
                const ULocalPlayer* const Player = GameEngine->GetFirstGamePlayer(World);
                if (Player)
                {
                    const AMahjongGameState* const GameState = Cast<AMahjongGameState>(Player->PlayerController->GetWorld()->GameState);

                    RankedPlayerMap Players;
                    GameState->PopulateRankedPlayerMap(Players);

                    bool bNeedsComma = false;
                    for (auto It = Players.CreateIterator(); It; ++It)
                    {
                        if (bNeedsComma)
                        {
                            ScoreboardString += TEXT(" ,");
                        }
                        ScoreboardString += FString::Printf(TEXT(" { \"Name\" : \"%s\", \"Score\" : \"%d\" }"), *It.Value()->GetShortPlayerName(), It.Value()->GetScore());
                        bNeedsComma = true;
                    }
                }

                ScoreboardString += TEXT(" ] }");

                Response.Add(TEXT("Content-Type"), TEXT("text/html; charset=utf-8"));
                Response.Add(TEXT("Body"), ScoreboardString);
            }
        }
    }
}

static void MahjongAssignStreamingChunkDelegate(const FString& PackageToAdd, const FString& LastLoadedMapName, const TArray<int32>& AssetRegistryChunkIDs, const TArray<int32>& ExistingChunkIds, TArray<int32>& OutChunkIndex)
{
    const int32 BasePak = 0;

    // Only add non-engine content to their own paks.
    if (!PackageToAdd.StartsWith("/Engine/"))
    {
        if ((LastLoadedMapName.Find(TEXT("BambooPark")) > 0 || PackageToAdd.Find(TEXT("BambooPark")) >= 0) &&
            !ExistingChunkIds.Contains(BasePak))
        {
            OutChunkIndex.Add(static_cast<int32>(EMahjongMap::BAMBOO_PARK));
        }
        // else if (Add more maps here when they are added)
    }

    if (OutChunkIndex.Num() == 0)
    {
        OutChunkIndex.Add(BasePak);
    }
}

static void MahjongAssignLayerChunkDelegate(const FAssignLayerChunkMap* ChunkManifest, const FString& Platform, const int32 ChunkIndex, int32& OutChunkLayer)
{
    OutChunkLayer = 0;

    // PS4 ONLY
    // I do not know exactly when this method is invoked, so I don't think I can do a preprocessor PLATFORM_PS4 check.

    static FString PS4PlatformString(TEXT("PS4"));
    if (Platform.Compare(TEXT("PS4"), ESearchCase::IgnoreCase) == 0)
    {
        // Dual layer BS50 packaging.
        switch (ChunkIndex)
        {
            case 0:
            case 1:
            default:
                OutChunkLayer = 0;
                break;
            case 2:
                OutChunkLayer = 1;
                break;
        }
    }
}

static void MahjongExtendedSaveGameInfoDelegate(const TCHAR* SaveName, const EGameDelegates_SaveGame Key, FString& Value)
{
    static const int32 MAX_SAVEGAME_SIZE = 100 * 1024;
    switch (Key)
    {
        case EGameDelegates_SaveGame::MaxSize:
            Value = FString::FromInt(MAX_SAVEGAME_SIZE);
            break;
        case EGameDelegates_SaveGame::Title:
            Value = TEXT("Mahjong");
            break;
        case EGameDelegates_SaveGame::SubTitle:
            Value = TEXT("The Mahjong Board Game Simulator");
            break;
        case EGameDelegates_SaveGame::Detail:
            Value = TEXT("Mahjong User Settings");
            break;
        default:
            break;
    }
}

void InitializeMahjongGameDelegates()
{
    FGameDelegates::Get().GetWebServerActionDelegate() = FWebServerActionDelegate::CreateStatic(MahjongWebServerDelegate);
    FGameDelegates::Get().GetAssignStreamingChunkDelegate() = FAssignStreamingChunkDelegate::CreateStatic(MahjongAssignStreamingChunkDelegate);
    FGameDelegates::Get().GetAssignLayerChunkDelegate() = FAssignLayerChunkDelegate::CreateStatic(MahjongAssignLayerChunkDelegate);
    FGameDelegates::Get().GetExtendedSaveGameInfoDelegate() = FExtendedSaveGameInfoDelegate::CreateStatic(MahjongExtendedSaveGameInfoDelegate);
}