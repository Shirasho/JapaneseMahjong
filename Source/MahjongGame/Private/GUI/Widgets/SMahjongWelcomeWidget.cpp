// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "SMahjongWelcomeWidget.h"

#include "MahjongWelcomeMenu.h"

FSlateColor SMahjongWelcomeWidget::GetTextColor() const
{
    return FSlateColor(FMath::Lerp(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), FLinearColor(0.5f, 0.5f, 0.5f, 1.0f), TextColorCurve.GetLerp()));
}

void SMahjongWelcomeWidget::Construct(const FArguments& InArgs)
{
    MenuOwner = InArgs._MenuOwner;

    TextAnimation = FCurveSequence();
    const float AnimDuration = 1.5f;
    TextColorCurve = TextAnimation.AddCurve(0, AnimDuration, ECurveEaseFunction::QuadInOut);

    //@TODO Build Widget
}

void SMahjongWelcomeWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    if (!TextAnimation.IsPlaying())
    {
        if (TextAnimation.IsAtEnd())
        {
            TextAnimation.PlayReverse(this->AsShared());
        }
        else
        {
            TextAnimation.Play(this->AsShared());
        }
    }
}

FReply SMahjongWelcomeWidget::OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return FReply::Handled();
}

FReply SMahjongWelcomeWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    const FKey Key = InKeyEvent.GetKey();
    if (Key == EKeys::Enter)
    {
        MenuOwner->HandleLoginUIClosed(TSharedPtr<const FUniqueNetId>(), 0);
    }
    else if (!MenuOwner->GetControlsLocked() && Key == EKeys::Gamepad_FaceButton_Bottom)
    {
        bool bSkipToMainMenu = true;

        {
            const auto OnlineSub = IOnlineSubsystem::Get();
            if (OnlineSub)
            {
                const auto IdentityInterface = OnlineSub->GetIdentityInterface();
                if (IdentityInterface.IsValid())
                {
                    TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
                    const bool bIsLicensed = GenericApplication->ApplicationLicenseValid();

                    const auto LoginStatus = IdentityInterface->GetLoginStatus(InKeyEvent.GetUserIndex());
                    if (LoginStatus == ELoginStatus::NotLoggedIn || !bIsLicensed)
                    {
                        // Show the account picker.
                        const auto ExternalUI = OnlineSub->GetExternalUIInterface();
                        if (ExternalUI.IsValid())
                        {
                            ExternalUI->ShowLoginUI(InKeyEvent.GetUserIndex(), false, FOnLoginUIClosedDelegate::CreateSP(MenuOwner, &FMahjongWelcomeMenu::HandleLoginUIClosed));
                            bSkipToMainMenu = false;
                        }
                    }
                }
            }
        }

        if (bSkipToMainMenu)
        {
            const auto OnlineSub = IOnlineSubsystem::Get();
            if (OnlineSub)
            {
                const auto IdentityInterface = OnlineSub->GetIdentityInterface();
                if (IdentityInterface.IsValid())
                {
                    TSharedPtr<const FUniqueNetId> UserId = IdentityInterface->GetUniquePlayerId(InKeyEvent.GetUserIndex());
                    // If we couldn't show the external login UI for any reason, or if the user is
                    // already logged in, just advance to the main menu immediately.
                    MenuOwner->HandleLoginUIClosed(UserId, InKeyEvent.GetUserIndex());
                }
            }
        }

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply SMahjongWelcomeWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
    return FReply::Handled().ReleaseMouseCapture().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly, true);
}

bool SMahjongWelcomeWidget::SupportsKeyboardFocus() const
{
    return true;
}