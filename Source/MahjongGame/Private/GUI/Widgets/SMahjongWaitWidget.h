// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

class SMahjongWaitWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMahjongWaitWidget)
    {
    }
    SLATE_ARGUMENT(FText, MessageText)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:

    /** our curve sequence and the related handles */
    FCurveSequence WidgetAnimation;

    /** used for animating the text color. */
    FCurveHandle TextColorCurve;

    /** Gets the animated text color */
    FSlateColor GetTextColor() const;
};