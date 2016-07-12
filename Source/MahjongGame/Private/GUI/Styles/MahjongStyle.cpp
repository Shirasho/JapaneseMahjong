// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongStyle.h"

#include "SlateGameResources.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".ttf"), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".otf"), __VA_ARGS__ )

TSharedPtr<FSlateStyleSet> FMahjongStyle::MahjongStyleInstance = nullptr;

void FMahjongStyle::Initialize()
{
    if (!MahjongStyleInstance.IsValid())
    {
        MahjongStyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle(*MahjongStyleInstance);
    }
}

void FMahjongStyle::Shutdown()
{
    FSlateStyleRegistry::UnRegisterSlateStyle(*MahjongStyleInstance);
    ensure(MahjongStyleInstance.IsUnique());
    MahjongStyleInstance.Reset();
}

FName FMahjongStyle::GetStyleSetName()
{
    static FName StyleSetName(TEXT("MahjongStyle"));
    return StyleSetName;
}

TSharedRef<FSlateStyleSet> FMahjongStyle::Create()
{
    TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(FMahjongStyle::GetStyleSetName(), "/Game/GUI/Styles", "/Game/GUI/Styles");
    FSlateStyleSet& Style = StyleRef.Get();

    // ## This section will highlight errors with the macros since FSlateXXX will only "register" the copy constructors with intellisense, but they are fine. ##

    // The speaker icon to display when a user is talking.
    Style.Set(TEXT("MahjongGame.Images.Online.Speaker"), new IMAGE_BRUSH("Images/Online/SpeakerIcon", FVector2D(32, 32)));

    // The fonts used.
    Style.Set(TEXT("MahjongGame.Fonts.MenuMainStyle"), FTextBlockStyle()
        .SetFont(TTF_FONT("Fonts///@TODO FONT_NAME", 20))
        .SetColorAndOpacity(FLinearColor::White)
        .SetShadowOffset(FIntPoint(-1, 1))
    );



    return StyleRef;
}

void FMahjongStyle::ReloadTextures()
{
    FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FMahjongStyle::Get()
{
    return *MahjongStyleInstance;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT