// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

using UnrealBuildTool;

// This module must be loaded "PreLoadingScreen" in the .uproject file, otherwise it will not hook in time!

public class MahjongGameLoadingScreen : ModuleRules
{
    public MahjongGameLoadingScreen(TargetInfo Target)
    {
        PrivateIncludePaths.Add("../../JapaneseMahjong/Source/MahjongGameLoadingScreen/Private");

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "MoviePlayer",
                "Slate",
                "SlateCore",
                "InputCore"
            }
        );
    }
}
