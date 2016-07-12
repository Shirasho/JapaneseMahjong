// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "SMahjongLoadingWidget.h"

#include "SlateExtras.h"    // Required for SSafeZone and SThrobber
#include "MahjongLoadingScreenBrush.h"

void SMahjongLoadingWidget::Construct(const FArguments& InArgs)
{
    static const FName LoadingScreenName(TEXT("/Game/UI/Menu/LoadingScreen.LoadingScreen"));

    //since we are not using game styles here, just load one image
    LoadingScreenBrush = MakeShareable(new FMahjongGameLoadingScreenBrush(LoadingScreenName, FVector2D(1920, 1080)));

    ChildSlot
        [
            SNew(SOverlay)
            + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SImage)
            .Image(LoadingScreenBrush.Get())
        ]
    + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SSafeZone)
            .VAlign(VAlign_Bottom)
        .HAlign(HAlign_Right)
        .Padding(10.0f)
        .IsTitleSafe(true)
        [
            SNew(SThrobber)
            .Visibility(this, &SMahjongLoadingWidget::GetLoadIndicatorVisibility)
        ]
        ]
        ];
}
