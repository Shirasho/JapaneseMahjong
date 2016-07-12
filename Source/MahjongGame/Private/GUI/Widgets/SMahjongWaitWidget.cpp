// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "SMahjongWaitWidget.h"


void SMahjongWaitWidget::Construct(const FArguments& InArgs)
{
    //const FMahjongMenuItemStyle* ItemStyle = &FMahjongStyle::Get().GetWidgetStyle<FMahjongMenuItemStyle>("DefaultMahjongMenuItemStyle");
    //const FButtonStyle* ButtonStyle = &FMahjongStyle::Get().GetWidgetStyle<FButtonStyle>("DefaultMahjongButtonStyle");
    //@TODO Build


    //Setup a curve
    const float StartDelay = 0.0f;
    const float SecondDelay = 0.0f;
    const float AnimDuration = 2.0f;

    WidgetAnimation = FCurveSequence();
    TextColorCurve = WidgetAnimation.AddCurve(StartDelay + SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
    WidgetAnimation.Play(this->AsShared(), true);
}