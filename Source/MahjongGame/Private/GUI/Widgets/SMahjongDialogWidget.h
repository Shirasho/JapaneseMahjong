// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongTypes.h"

class SMahjongDialogWidget : public SCompoundWidget
{
public:
    /** The player that owns the dialog. */
    TWeakObjectPtr<ULocalPlayer> PlayerOwner;

    /** The delegate for confirming */
    FOnClicked OnConfirm;

    /** The delegate for cancelling */
    FOnClicked OnCancel;

    /* The type of dialog this is */
    EMahjongDialogType::Type DialogType;

    SLATE_BEGIN_ARGS(SMahjongDialogWidget)
    {
    }

    SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)
    SLATE_ARGUMENT(EMahjongDialogType::Type, DialogType)

    SLATE_ARGUMENT(FText, MessageText)
    SLATE_ARGUMENT(FText, ConfirmText)
    SLATE_ARGUMENT(FText, CancelText)

    SLATE_ARGUMENT(FOnClicked, OnConfirmClicked)
    SLATE_ARGUMENT(FOnClicked, OnCancelClicked)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    virtual bool SupportsKeyboardFocus() const override;
    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;

private:

    FReply OnConfirmHandler();
    FReply ExecuteConfirm(const int32 UserIndex);

};