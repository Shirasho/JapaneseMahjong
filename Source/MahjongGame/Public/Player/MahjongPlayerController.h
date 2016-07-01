// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongPlayerController.generated.h"

UCLASS(config=Game)
class AMahjongPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

protected:

	/** if set, gameplay related actions (movement, weapn usage, etc) are allowed */
	bool bAllowGameActions;

	/** true for the first frame after the game has ended */
	bool bGameEndedFrame;

	/** For tracking whether or not to send the end event */
	bool bHasSentStartEvents;

	FName ServerSayString;

private:

	/** Handle for efficient management of ClientStartOnlineGame timer */
	FTimerHandle TimerHandle_ClientStartOnlineGame;

public:

	/** Sets spectator location and rotation. */
	UFUNCTION(reliable, client)
	void ClientSetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation);

	/** Notify player about started match. */
	UFUNCTION(reliable, client)
	void ClientGameStarted();

	/** Starts the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientStartOnlineGame();

	/** Ends the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientEndOnlineGame();

	/** Notify player about finished match */
	virtual void ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner);

	/** Notifies clients to send the end-of-round event */
	UFUNCTION(reliable, client)
	void ClientSendRoundEndEvent(bool bIsWinner, int32 ExpendedTimeInSeconds);

	/** sends cheat message */
	UFUNCTION(reliable, server, WithValidation)
	void ServerCheat(const FString& Message);

	/* Overriden Message implementation. */
	virtual void ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& Message, FName Type, float MsgLifeTime) override;

	/** Local function say a string */
	UFUNCTION(exec)
	virtual void Say(const FString& Message);

	/** RPC for clients to talk to server */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerSay(const FString& Message);

	/** check if gameplay related actions (movement, weapon usage, etc) are allowed right now */
	bool IsGameInputAllowed() const;

	/** Returns the persistent user record associated with this player, or null if there is't one. */
	class UMahjongSaveGame* GetSaveGame() const;

	/** Associate a new UPlayer with this PlayerController. */
	virtual void SetPlayer(UPlayer* Player);

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	virtual void PawnPendingDestroy(APawn* InPawn) override;

	virtual void UnFreeze() override;

	virtual void SetupInputComponent() override;

	virtual void GameHasEnded(class AActor* EndGameFocus = NULL, bool bIsWinner = false) override;

	void UpdateSaveFileOnGameEnd(bool bIsWinner);
};