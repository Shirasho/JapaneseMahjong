// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once


class SMahjongMenuWidget : public SCompoundWidget
{

public:

    SLATE_BEGIN_ARGS(SMahjongMenuWidget)
        : _PlayerOwner()
        , _IsGameMenu(false)
    {
    }

    /** weak pointer to the parent HUD base */
    SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)

    /** is this main menu or in game menu? */
    SLATE_ARGUMENT(bool, IsGameMenu)

    /** always goes here */
    SLATE_END_ARGS()

    /** delegate declaration */
    DECLARE_DELEGATE(FOnMenuHidden);

    /** external delegate to call when in-game menu should be hidden using controller buttons -
    it's workaround as when joystick is captured, even when sending FReply::Unhandled, binding does not recieve input :( */
    DECLARE_DELEGATE(FOnToggleMenu);


private:

    /** if moving around menu is currently locked */
    bool bControlsLocked;

public:


    /** every widget needs a construction function */
    void Construct(const FArguments& InArgs);

    /** disable/enable moving around menu */
    void LockControls(bool bEnable);

    /** update function. Kind of a hack. Allows us to only start fading in once we are done loading. */
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    /** to have the mouse cursor show up at all times, we need the widget to handle all mouse events */
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    /** key down handler */
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

    /** says that we can support keyboard focus */
    virtual bool SupportsKeyboardFocus() const override { return true; }

    /** The menu sets up the appropriate mouse settings upon focus */
    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
};