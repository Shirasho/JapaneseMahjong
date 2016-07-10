// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameViewportClient.h"

#include "MahjongLocalPlayer.h"

#include "SMahjongDialogWidget.h"
#include "SMahjongLoadingWidget.h"


UMahjongGameViewportClient::UMahjongGameViewportClient(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetSuppressTransitionMessage(true);
}

void UMahjongGameViewportClient::NotifyPlayerAdded(int32 PlayerIndex, ULocalPlayer* AddedPlayer)
{
    Super::NotifyPlayerAdded(PlayerIndex, AddedPlayer);

    UMahjongLocalPlayer* const LocalPlayer = Cast<UMahjongLocalPlayer>(AddedPlayer);
    if (LocalPlayer)
    {
        LocalPlayer->LoadSaveGame();
    }
}

void UMahjongGameViewportClient::AddViewportWidgetContent(TSharedRef<class SWidget> ViewportContent, const int32 ZOrder)
{
    UE_LOG(LogPlayerManagement, Log, TEXT("UMahjongGameViewportClient::AddViewportWidgetContent: %p"), &ViewportContent.Get());

    if ((DialogWidget.IsValid() || LoadingWidget.IsValid()) && ViewportContent != DialogWidget && ViewportContent != LoadingWidget)
    {
        // Add to the hidden widget list, and don't show it until we hide the dialog widget.
        HiddenViewportContentStack.AddUnique(ViewportContent);
        return;
    }

    if (ViewportContentStack.Contains(ViewportContent))
    {
        return;
    }

    ViewportContentStack.AddUnique(ViewportContent);
    Super::AddViewportWidgetContent(ViewportContent, ZOrder);
}

void UMahjongGameViewportClient::RemoveViewportWidgetContent(TSharedRef<class SWidget> ViewportContent)
{
    UE_LOG(LogPlayerManagement, Log, TEXT("UMahjongGameViewportClient::RemoveViewportWidgetContent: %p"), &ViewportContent.Get());

    ViewportContentStack.Remove(ViewportContent);
    HiddenViewportContentStack.Remove(ViewportContent);

    Super::RemoveViewportWidgetContent(ViewportContent);
}

void UMahjongGameViewportClient::HideExistingWidgets()
{
    check(HiddenViewportContentStack.Num() == 0);

    TArray<TSharedRef<class SWidget>> CopyOfViewportContentStack = ViewportContentStack;

    for (int32 i = ViewportContentStack.Num() - 1; i >= 0; i--)
    {
        RemoveViewportWidgetContent(ViewportContentStack[i]);
    }

    HiddenViewportContentStack = CopyOfViewportContentStack;
}

void UMahjongGameViewportClient::ShowExistingWidgets()
{
    // We shouldn't have any visible widgets at this point
    check(ViewportContentStack.Num() == 0);

    // Unhide all of the previously hidden widgets
    for (int32 i = 0; i < HiddenViewportContentStack.Num(); i++)
    {
        AddViewportWidgetContent(HiddenViewportContentStack[i]);
    }

    check(ViewportContentStack.Num() == HiddenViewportContentStack.Num());

    // Done with these
    HiddenViewportContentStack.Empty();
}

void UMahjongGameViewportClient::ShowDialog(TWeakObjectPtr<ULocalPlayer> PlayerOwner, EMahjongDialogType::Type DialogType, const FText& Message, const FText& Confirm, const FText& Cancel, const FOnClicked& OnConfirm, const FOnClicked& OnCancel)
{
    UE_LOG(LogPlayerManagement, Log, TEXT("UMahjongGameViewportClient::ShowDialog..."));

    if (DialogWidget.IsValid())
    {
        return;	// Already showing a dialog box
    }

    // Hide all existing widgets
    if (!LoadingWidget.IsValid())
    {
        HideExistingWidgets();
    }

    DialogWidget = SNew(SMahjongDialogWidget)
        .PlayerOwner(PlayerOwner)
        .DialogType(DialogType)
        .MessageText(Message)
        .ConfirmText(Confirm)
        .CancelText(Cancel)
        .OnConfirmClicked(OnConfirm)
        .OnCancelClicked(OnCancel);

    if (LoadingWidget.IsValid())
    {
        // Can't show dialog while loading screen is visible
        HiddenViewportContentStack.Add(DialogWidget.ToSharedRef());
    }
    else
    {
        AddViewportWidgetContent(DialogWidget.ToSharedRef());

        // Remember what widget currently has focus
        OldFocusWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();

        // Force focus to the dialog widget
        FSlateApplication::Get().SetKeyboardFocus(DialogWidget, EFocusCause::SetDirectly);
    }
}

void UMahjongGameViewportClient::HideDialog()
{
    UE_LOG(LogPlayerManagement, Log, TEXT("UMahjongGameViewportClient::HideDialog. DialogWidget: %p, OldFocusWidget: %p"), DialogWidget.Get(), OldFocusWidget.Get());

    if (DialogWidget.IsValid())
    {
        const bool bRestoreOldFocus = OldFocusWidget.IsValid() && FSlateApplication::Get().GetKeyboardFocusedWidget() == DialogWidget;

        // Hide the dialog widget
        RemoveViewportWidgetContent(DialogWidget.ToSharedRef());

        // Destroy the dialog widget
        DialogWidget = NULL;

        if (!LoadingWidget.IsValid())
        {
            ShowExistingWidgets();
        }

        // Restore focus to last widget (but only if the dialog currently has focus still)
        if (bRestoreOldFocus)
        {
            FSlateApplication::Get().SetKeyboardFocus(OldFocusWidget, EFocusCause::SetDirectly);
        }

        OldFocusWidget = NULL;
    }
}

void UMahjongGameViewportClient::ShowLoadingScreen()
{
    if (LoadingWidget.IsValid())
    {
        return;
    }

    if (DialogWidget.IsValid())
    {
        // Hide the dialog widget (loading screen takes priority)
        check(!HiddenViewportContentStack.Contains(DialogWidget.ToSharedRef()));
        check(ViewportContentStack.Contains(DialogWidget.ToSharedRef()));
        RemoveViewportWidgetContent(DialogWidget.ToSharedRef());
        HiddenViewportContentStack.Add(DialogWidget.ToSharedRef());
    }
    else
    {
        // Hide all existing widgets
        HideExistingWidgets();
    }

    LoadingWidget = SNew(SMahjongLoadingWidget);

    AddViewportWidgetContent(LoadingWidget.ToSharedRef());
}

void UMahjongGameViewportClient::HideLoadingScreen()
{
    if (!LoadingWidget.IsValid())
    {
        return;
    }

    RemoveViewportWidgetContent(LoadingWidget.ToSharedRef());

    LoadingWidget = NULL;

    // Show the dialog widget if we need to
    if (DialogWidget.IsValid())
    {
        check(HiddenViewportContentStack.Contains(DialogWidget.ToSharedRef()));
        check(!ViewportContentStack.Contains(DialogWidget.ToSharedRef()));
        HiddenViewportContentStack.Remove(DialogWidget.ToSharedRef());
        AddViewportWidgetContent(DialogWidget.ToSharedRef());
    }
    else
    {
        ShowExistingWidgets();
    }
}

EMahjongDialogType::Type UMahjongGameViewportClient::GetDialogType() const
{
    return (DialogWidget.IsValid() ? DialogWidget->DialogType : EMahjongDialogType::None);
}

TWeakObjectPtr<ULocalPlayer> UMahjongGameViewportClient::GetDialogOwner() const
{
    return (DialogWidget.IsValid() ? DialogWidget->PlayerOwner : nullptr);
}

void UMahjongGameViewportClient::Tick(float DeltaSeconds)
{
    if (DialogWidget.IsValid() && !LoadingWidget.IsValid())
    {
        // Make sure the dialog widget always has focus
        if (FSlateApplication::Get().GetKeyboardFocusedWidget() != DialogWidget)
        {
            // Remember which widget had focus before we override it
            OldFocusWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();

            // Force focus back to dialog
            FSlateApplication::Get().SetKeyboardFocus(DialogWidget, EFocusCause::SetDirectly);
        }
    }
}

#if WITH_EDITOR
void UMahjongGameViewportClient::DrawTransition(UCanvas* Canvas)
{
    if (GetOuterUEngine() != NULL)
    {
        TEnumAsByte<enum ETransitionType> Type = GetOuterUEngine()->TransitionType;
        switch (Type)
        {
            case TT_Connecting:
                DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "ConnectingMessage", "CONNECTING").ToString());
                break;
            case TT_WaitingToConnect:
                DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "Waitingtoconnect", "Waiting to connect...").ToString());
                break;
        }
    }
}
#endif //WITH_EDITOR