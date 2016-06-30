// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "AssetRegistryModule.h"

class FMahjongGameModule : public FDefaultGameModuleImpl {
	
	virtual void StartupModule() override {
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		//Hot reload hack
		//@TODO
		//FSlateStyleRegistry::UnRegisterSlateStyle(FMahjongStyle::GetStyleSetName());
		//FMahjongStyle::Initialize();
	}

	virtual void ShutdownModule() override {
		//@TODO
		//FMahjongStyle::Shutdown();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FMahjongGameModule, MahjongGame, "MahjongGame" );

DEFINE_LOG_CATEGORY(LogMahjong);
DEFINE_LOG_CATEGORY(LogMahjongPlayer);
DEFINE_LOG_CATEGORY(LogMahjongServer);
DEFINE_LOG_CATEGORY(LogMahjongHand);
