// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

using UnrealBuildTool;

public class MahjongGame : ModuleRules
{
	public MahjongGame(TargetInfo Target)
	{
        PrivateIncludePaths.AddRange(
            new string[]
            {
                "MahjongGame/Private/GUI",
                "MahjongGame/Private/GUI/Menus",
                "MahjongGame/Private/GUI/Styles",
                "MahjongGame/Private/GUI/Widgets",
            }
        );

		PublicDependencyModuleNames.AddRange(
            new string[] 
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UMG",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
            }
        );
        
		PrivateDependencyModuleNames.AddRange(
            new string[] {
                "InputCore",
                "Slate",
                "SlateCore",
                "Json",
                "MahjongGameLoadingScreen"
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "OnlineSubsystemNull",
                "NetworkReplayStreaming",
                "NullNetworkReplayStreaming",
                "HttpNetworkReplayStreaming"
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "NetworkReplayStreaming"
            }
        );
        
        if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        {
            if (UEBuildConfiguration.bCompileSteamOSS == true)
            {
                DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
            }
        }
    }
}
