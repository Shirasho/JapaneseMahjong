// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongPlayerController_Menu.h"

#include "MahjongStyle.h"

AMahjongPlayerController_Menu::AMahjongPlayerController_Menu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AMahjongPlayerController_Menu::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FMahjongStyle::Initialize();
}