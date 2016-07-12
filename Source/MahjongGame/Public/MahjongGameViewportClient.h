// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongTypes.h"
#include "MahjongGameViewportClient.generated.h"

class SMahjongLoadingWidget;
class SMahjongDialogWidget;

UCLASS(Within=Engine, transient, config=Engine)
class UMahjongGameViewportClient : public UGameViewportClient
{
    GENERATED_UCLASS_BODY()

public:

    void NotifyPlayerAdded(int32 PlayerIndex, ULocalPlayer* AddedPlayer) override;
    void AddViewportWidgetContent(TSharedRef<class SWidget> ViewportContent, const int32 ZOrder = 0) override;
    void RemoveViewportWidgetContent(TSharedRef<class SWidget> ViewportContent) override;

    void ShowDialog(TWeakObjectPtr<ULocalPlayer> PlayerOwner, EMahjongDialogType::Type DialogType, const FText& Message, const FText& Confirm, const FText& Cancel, const FOnClicked& OnConfirm, const FOnClicked& OnCancel);
    void HideDialog();

    void ShowLoadingScreen();
    void HideLoadingScreen();

    bool IsShowingDialog() const { return DialogWidget.IsValid(); }

    EMahjongDialogType::Type GetDialogType() const;
    TWeakObjectPtr<ULocalPlayer> GetDialogOwner() const;

    TSharedPtr<SMahjongDialogWidget> GetDialogWidget() { return DialogWidget; }

    virtual void Tick(float DeltaSeconds) override;

#if WITH_EDITOR
    virtual void DrawTransition(class UCanvas* Canvas) override;
#endif

protected:
    void HideExistingWidgets();
    void ShowExistingWidgets();

    /** List of viewport content that the viewport is tracking */
    TArray<TSharedRef<class SWidget>>				ViewportContentStack;

    TArray<TSharedRef<class SWidget>>				HiddenViewportContentStack;

    TSharedPtr<class SWidget>						OldFocusWidget;

    /** Dialog widget to show temporary messages ("Controller disconnected", "Parental Controls don't allow you to play online", etc) */
    TSharedPtr<SMahjongDialogWidget>	DialogWidget;

    TSharedPtr<SMahjongLoadingWidget>	LoadingWidget;
};