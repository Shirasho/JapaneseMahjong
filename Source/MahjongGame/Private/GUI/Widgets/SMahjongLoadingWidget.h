// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

class SMahjongLoadingWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMahjongLoadingWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    EVisibility GetLoadIndicatorVisibility() const
    {
        return EVisibility::Visible;
    }

    /** loading screen image brush */
    TSharedPtr<FSlateDynamicImageBrush> LoadingScreenBrush;
};