// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "SMahjongDialogWidget.h"


void SMahjongDialogWidget::Construct(const FArguments& InArgs)
{
    PlayerOwner = InArgs._PlayerOwner;
    DialogType = InArgs._DialogType;

    OnConfirm = InArgs._OnConfirmClicked;
    OnCancel = InArgs._OnCancelClicked;

    //@TODO Build
}

bool SMahjongDialogWidget::SupportsKeyboardFocus() const
{
    return true;
}

FReply SMahjongDialogWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
    return FReply::Handled().ReleaseMouseCapture().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly, true);
}

FReply SMahjongDialogWidget::OnConfirmHandler()
{
    return ExecuteConfirm(FSlateApplication::Get().GetUserIndexForKeyboard());
}

FReply SMahjongDialogWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    const FKey Key = KeyEvent.GetKey();
    const int32 UserIndex = KeyEvent.GetUserIndex();

    // Filter input based on the type of this dialog
    switch (DialogType)
    {
        case EMahjongDialogType::Generic:
        {
            // Ignore input from users that don't own this dialog
            if (PlayerOwner != nullptr && PlayerOwner->GetControllerId() != UserIndex)
            {
                return FReply::Unhandled();
            }
            break;
        }

        case EMahjongDialogType::ControllerDisconnected:
        {
            // Only ignore input from controllers that are bound to local users
            if (PlayerOwner != nullptr && PlayerOwner->GetGameInstance() != nullptr)
            {
                if (PlayerOwner->GetGameInstance()->FindLocalPlayerFromControllerId(UserIndex))
                {
                    return FReply::Unhandled();
                }
            }
            break;
        }
    }

    // For testing on PC
    if ((Key == EKeys::Enter || Key == EKeys::Gamepad_FaceButton_Bottom) && !KeyEvent.IsRepeat())
    {
        return ExecuteConfirm(UserIndex);
    }
    else if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right)
    {
        if (OnCancel.IsBound())
        {
            return OnCancel.Execute();
        }
    }

    return FReply::Unhandled();
}

FReply SMahjongDialogWidget::ExecuteConfirm(const int32 UserIndex)
{
    if (OnConfirm.IsBound())
    {
        //these two cases should be combined when we move to using PlatformUserIDs rather than ControllerIDs.
#if PLATFORM_PS4
        bool bExecute = false;
        // For controller reconnection, bind the confirming controller to the owner of this dialog
        if (DialogType == EMahjongDialogType::ControllerDisconnected && PlayerOwner != nullptr)
        {
            IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
            if (OnlineSub)
            {
                const auto IdentityInterface = OnlineSub->GetIdentityInterface();
                if (IdentityInterface.IsValid())
                {
                    TSharedPtr<const FUniqueNetId> IncomingUserId = IdentityInterface->GetUniquePlayerId(UserIndex);
                    TSharedPtr<const FUniqueNetId> DisconnectedId = PlayerOwner->GetCachedUniqueNetId();

                    if (*IncomingUserId == *DisconnectedId)
                    {
                        PlayerOwner->SetControllerId(UserIndex);
                        bExecute = true;
                    }
                }
            }
        }
        else
        {
            bExecute = true;
        }

        if (bExecute)
        {
            return OnConfirm.Execute();
        }
#else
        // For controller reconnection, bind the confirming controller to the owner of this dialog
        if (DialogType == EMahjongDialogType::ControllerDisconnected && PlayerOwner != nullptr)
        {
            PlayerOwner->SetControllerId(UserIndex);
        }

        return OnConfirm.Execute();
#endif
    }

    return FReply::Unhandled();
}