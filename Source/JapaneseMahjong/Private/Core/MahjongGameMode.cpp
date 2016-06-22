// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"			// Required include
#include "MahjongGameMode.h"			// Required include

#include "Blueprint/UserWidget.h"

#include "MahjongPlayerController.h"
#include "MahjongGameState.h"


AMahjongGameMode::AMahjongGameMode() : Super() {

	DefaultPawnClass = nullptr;
	PlayerControllerClass = AMahjongPlayerController::StaticClass();
	GameStateClass = AMahjongGameState::StaticClass();
}

void AMahjongGameMode::BeginPlay() {
	Super::BeginPlay();
	AddMenuWidget(StartingWidgetClass);
}

UUserWidget* AMahjongGameMode::AddMenuWidget(TSubclassOf<UUserWidget> WidgetToAdd) {
	if (WidgetToAdd) {
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), WidgetToAdd);
		Widget->AddToViewport();
		ActiveWidgets.Add(Widget);
		//@TODO Will this return nullptr due to Widget being destroyed at end of scope,
		// or will the reference still be valid by adding it to the ActiveWidgets array?
		return Widget;
	}
	return nullptr;
}

void AMahjongGameMode::RemoveMenuWidget(UUserWidget* WidgetToRemove) {
	if (WidgetToRemove) {
		ActiveWidgets.Remove(WidgetToRemove);
		WidgetToRemove->RemoveFromViewport();
	}
}