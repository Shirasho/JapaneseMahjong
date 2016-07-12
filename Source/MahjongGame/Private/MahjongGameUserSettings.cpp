// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongGameUserSettings.h"

UMahjongGameUserSettings::UMahjongGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetToDefaults();
}

void UMahjongGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	GraphicsQuality = 1;
    ResolutionScale = 100;
	bIsLanMatch = true;
}

void UMahjongGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
    // Overall quality.
	ScalabilityQuality.SetFromSingleQualityLevel(3);

    // Resolution scale.
    ScalabilityQuality.ResolutionQuality = ResolutionScale;

	Super::ApplySettings(bCheckForCommandLineOverrides);
}