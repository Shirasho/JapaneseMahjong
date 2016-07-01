// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongLocalPlayer.h"

#include "MahjongSaveGame.h"
#include "MahjongGameInstance.h"
#include "PlayerDefinitions.h"

#include "Online.h"
//#include "OnlineSubsystemUtilsClasses.h"


UMahjongLocalPlayer::UMahjongLocalPlayer() : Super()
{

}

UMahjongSaveGame* UMahjongLocalPlayer::GetSaveGame() const
{
	if (!SaveGame)
	{
		// Cast away constness to enable caching.
		UMahjongLocalPlayer* MutableThis = const_cast<UMahjongLocalPlayer*>(this);
		MutableThis->LoadSaveGame();
	}

	return SaveGame;
}

void UMahjongLocalPlayer::LoadSaveGame()
{
	// If we changed the controller id or user, we need to load the new save.
	if (SaveGame && (GetControllerId() != SaveGame->GetUserIndex() || GetNickname() != SaveGame->GetName()))
	{
		SaveGame->SaveIfDirty();
		SaveGame = nullptr;
	}

	if (!SaveGame)
	{
		FPlatformUserId PlatformId = GetControllerId();
		auto Identity = Online::GetIdentityInterface();
		if (Identity.IsValid() && GetPreferredUniqueNetId().IsValid())
		{
			PlatformId = Identity->GetPlatformUserIdFromUniqueNetId(*GetPreferredUniqueNetId());
		}

		SaveGame = UMahjongSaveGame::LoadSaveGame(GetNickname(), PlatformId);
	}
}

void UMahjongLocalPlayer::SetControllerId(int32 NewControllerId)
{
	//ULocalPlayer::SetControllerId(NewControllerId);
	Super::SetControllerId(NewControllerId);

	LoadSaveGame();
}

FString UMahjongLocalPlayer::GetNickname() const
{
	FString UserHandle = Super::GetNickname();

	if (UserHandle.Len() > PLAYER_NAME_LENGTH_MAX)
	{
		UserHandle = UserHandle.Left(PLAYER_NAME_LENGTH_MAX) + TEXT("...");
	}

	bool bReplace = (UserHandle.Len() == 0);

	// Check for duplicate nicknames.
	static bool bReentry = false;
	if (!bReentry)
	{
		bReentry = true;

		UMahjongGameInstance* GameInstance = GetWorld() ? Cast<UMahjongGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
		if (GameInstance)
		{
			// Check all names that occur before ours that are the same.
			const TArray<ULocalPlayer*>& LocalPlayers = GameInstance->GetLocalPlayers();

			for (const ULocalPlayer* LocalPlayer : LocalPlayers)
			{
				if (LocalPlayer == this)
				{
					break;
				}

				if (UserHandle == LocalPlayer->GetNickname())
				{
					bReplace = true;
					break;
				}
			}
		}
		bReentry = false;
	}

	if (bReplace)
	{
		UserHandle = FString::Printf(TEXT("Player%i"), GetControllerId() + 1);
	}

	return UserHandle;
}