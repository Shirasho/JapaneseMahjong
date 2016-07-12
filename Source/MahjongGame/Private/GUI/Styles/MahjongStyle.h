// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

class FMahjongStyle
{
public:

    static void Initialize();

    static void Shutdown();

    /** reloads textures used by slate renderer */
    static void ReloadTextures();

    static const ISlateStyle& Get();

    static FName GetStyleSetName();

private:

    static TSharedRef<class FSlateStyleSet> Create();

private:

    static TSharedPtr<class FSlateStyleSet> MahjongStyleInstance;
};