// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

class SMahjongWelcomeWidget : public SCompoundWidget
{
    /** The menu that owns this widget. */
    FMahjongWelcomeMenu* MenuOwner;

    /** Animate the text so that the screen isn't static, for console cert requirements. */
    FCurveSequence TextAnimation;

    /** The actual curve that animates the text. */
    FCurveHandle TextColorCurve;

    SLATE_BEGIN_ARGS(SMahjongWelcomeWidget)
    {
    }

    SLATE_ARGUMENT(FMahjongWelcomeMenu*, MenuOwner)

    SLATE_END_ARGS()

public:

    virtual bool SupportsKeyboardFocus() const override;

    FSlateColor GetTextColor() const;

    void Construct(const FArguments& InArgs);

    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    virtual FReply OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
};