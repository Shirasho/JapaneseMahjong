// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongWelcomeMenu.h"

#include "OnlineDefinitions.h"

#include "MahjongGameInstance.h"
#include "MahjongGameViewportClient.h"

#include "SMahjongWelcomeWidget.h"


void FMahjongWelcomeMenu::Construct(TWeakObjectPtr<UMahjongGameInstance> InGameInstance)
{
    bControlsLocked = false;
    GameInstance = InGameInstance;
    PendingControllerIndex = -1;
    
    MenuWidget = SNew(SMahjongWelcomeWidget)
        .MenuOwner(this);
}

void FMahjongWelcomeMenu::AddToGameViewport()
{
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->AddViewportWidgetContent(MenuWidget.ToSharedRef());
        FSlateApplication::Get().SetKeyboardFocus(MenuWidget);
    }
}

void FMahjongWelcomeMenu::RemoveFromGameViewport()
{
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(MenuWidget.ToSharedRef());
    }
}

void FMahjongWelcomeMenu::HandleLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex)
{
    if (!ensure(GameInstance.IsValid()))
    {
        return;
    }

    UMahjongGameViewportClient* MahjongViewport = Cast<UMahjongGameViewportClient>(GameInstance->GetGameViewportClient());

    TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
    const bool bIsLicensed = GenericApplication->ApplicationLicenseValid();

    // If they don't currently have a license, let them know, but don't let them proceed
    if (MAHJONG_ONLINE_LICENSE_REQUIRED && !bIsLicensed && MahjongViewport != NULL)
    {
        const FText StopReason = NSLOCTEXT("ProfileMessages", "NeedLicense", "The signed in users do not have a license for this game. Please purchase the game from a verified retailer or sign in a user with a valid license.");
        const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

        MahjongViewport->ShowDialog(
            nullptr,
            EMahjongDialogType::Generic,
            StopReason,
            OKButton,
            FText::GetEmpty(),
            FOnClicked::CreateRaw(this, &FMahjongWelcomeMenu::OnConfirmGeneric),
            FOnClicked::CreateRaw(this, &FMahjongWelcomeMenu::OnConfirmGeneric)
        );
        return;
    }

    PendingControllerIndex = ControllerIndex;

    if (UniqueId.IsValid())
    {
        // Next step, check privileges
        const auto OnlineSub = IOnlineSubsystem::Get();
        if (OnlineSub)
        {
            const auto IdentityInterface = OnlineSub->GetIdentityInterface();
            if (IdentityInterface.IsValid())
            {
                IdentityInterface->GetUserPrivilege(*UniqueId, EUserPrivileges::CanPlay, IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateSP(this, &FMahjongWelcomeMenu::OnUserCanPlay));
            }
        }
    }
    else
    {
        // Show a warning that your progress won't be saved if you continue without logging in. 
        if (MahjongViewport != NULL)
        {
            MahjongViewport->ShowDialog(
                nullptr,
                EMahjongDialogType::Generic,
                NSLOCTEXT("ProfileMessages", "ProgressWillNotBeSaved", "If you continue without signing in, your progress will not be saved."),
                NSLOCTEXT("DialogButtons", "AContinue", "A - Continue"),
                NSLOCTEXT("DialogButtons", "BBack", "B - Back"),
                FOnClicked::CreateRaw(this, &FMahjongWelcomeMenu::OnContinueWithoutSavingConfirm),
                FOnClicked::CreateRaw(this, &FMahjongWelcomeMenu::OnConfirmGeneric)
            );
        }
    }
}

void FMahjongWelcomeMenu::SetControllerAndAdvanceToMainMenu(const int ControllerIndex)
{
    if (!ensure(GameInstance.IsValid()))
    {
        return;
    }

    ULocalPlayer * NewPlayerOwner = GameInstance->GetFirstGamePlayer();

    if (NewPlayerOwner != nullptr && ControllerIndex != -1)
    {
        NewPlayerOwner->SetControllerId(ControllerIndex);
        NewPlayerOwner->SetCachedUniqueNetId(NewPlayerOwner->GetUniqueNetIdFromCachedControllerId());

        // tell gameinstance to transition to main menu
        GameInstance->GotoState(MahjongGameInstanceState::MainMenu);
    }
}

FReply FMahjongWelcomeMenu::OnContinueWithoutSavingConfirm()
{
    if (!ensure(GameInstance.IsValid()))
    {
        return FReply::Handled();
    }

    UMahjongGameViewportClient * MahjongViewport = Cast<UMahjongGameViewportClient>(GameInstance->GetGameViewportClient());

    if (MahjongViewport)
    {
        MahjongViewport->HideDialog();
    }

    SetControllerAndAdvanceToMainMenu(PendingControllerIndex);
    return FReply::Handled();
}

FReply FMahjongWelcomeMenu::OnConfirmGeneric()
{
    if (!ensure(GameInstance.IsValid()))
    {
        return FReply::Handled();
    }

    UMahjongGameViewportClient * MahjongViewport = Cast<UMahjongGameViewportClient>(GameInstance->GetGameViewportClient());

    if (MahjongViewport)
    {
        MahjongViewport->HideDialog();
    }

    return FReply::Handled();
}

void FMahjongWelcomeMenu::OnUserCanPlay(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
    if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
    {
        SetControllerAndAdvanceToMainMenu(PendingControllerIndex);
    }
    else
    {
        UMahjongGameViewportClient * MahjongViewport = Cast<UMahjongGameViewportClient>(GameInstance->GetGameViewportClient());

        if (MahjongViewport)
        {
            const FText ReturnReason = NSLOCTEXT("PrivilegeFailures", "CannotPlayAgeRestriction", "You cannot play this game due to age restrictions.");
            const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

            MahjongViewport->ShowDialog(
                nullptr,
                EMahjongDialogType::Generic,
                ReturnReason,
                OKButton,
                FText::GetEmpty(),
                FOnClicked::CreateRaw(this, &FMahjongWelcomeMenu::OnConfirmGeneric),
                FOnClicked::CreateRaw(this, &FMahjongWelcomeMenu::OnConfirmGeneric)
            );
        }
    }
}

