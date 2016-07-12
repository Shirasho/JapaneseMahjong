// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongMessageMenu.h"

#include "MahjongGameInstance.h"
#include "MahjongGameViewportClient.h"


void FMahjongMessageMenu::Construct(TWeakObjectPtr<UMahjongGameInstance> InGameInstance, TWeakObjectPtr<ULocalPlayer> InPlayerOwner, const FText& Message, const FText& OKButtonText, const FText& CancelButtonText, const FName& InPendingNextState)
{
    GameInstance = InGameInstance;
    PlayerOwner = InPlayerOwner;
    PendingNextState = InPendingNextState;

    if (ensure(GameInstance.IsValid()))
    {
        UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GameInstance->GetGameViewportClient());

        if (MahjongViewport)
        {
            // Hide the previous dialog
            MahjongViewport->HideDialog();

            // Show the new one
            MahjongViewport->ShowDialog(
                PlayerOwner,
                EMahjongDialogType::Generic,
                Message,
                OKButtonText,
                CancelButtonText,
                FOnClicked::CreateRaw(this, &FMahjongMessageMenu::OnClickedOK),
                FOnClicked::CreateRaw(this, &FMahjongMessageMenu::OnClickedCancel)
            );
        }
    }
}

void FMahjongMessageMenu::RemoveFromGameViewport()
{
    if (ensure(GameInstance.IsValid()))
    {
        UMahjongGameViewportClient * MahjongViewport = Cast<UMahjongGameViewportClient>(GameInstance->GetGameViewportClient());

        if (MahjongViewport)
        {
            // Hide the previous dialog
            MahjongViewport->HideDialog();
        }
    }
}

void FMahjongMessageMenu::HideDialogAndGotoNextState()
{
    RemoveFromGameViewport();

    if (ensure(GameInstance.IsValid()))
    {
        GameInstance->GotoState(PendingNextState);
    }
};

FReply FMahjongMessageMenu::OnClickedOK()
{
    OKButtonDelegate.ExecuteIfBound();
    HideDialogAndGotoNextState();
    return FReply::Handled();
}

FReply FMahjongMessageMenu::OnClickedCancel()
{
    CancelButtonDelegate.ExecuteIfBound();
    HideDialogAndGotoNextState();
    return FReply::Handled();
}

void FMahjongMessageMenu::SetOKClickedDelegate(FMessageMenuButtonClicked InButtonDelegate)
{
    OKButtonDelegate = InButtonDelegate;
}

void FMahjongMessageMenu::SetCancelClickedDelegate(FMessageMenuButtonClicked InButtonDelegate)
{
    CancelButtonDelegate = InButtonDelegate;
}