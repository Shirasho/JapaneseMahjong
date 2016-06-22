// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "GameFramework/GameMode.h"
#include "MahjongGameMode.generated.h"

class UUserWidget;

UCLASS(abstract)
class JAPANESEMAHJONG_API AMahjongGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:

	AMahjongGameMode();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GUI")
	TSubclassOf<UUserWidget> StartingWidgetClass;

public:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "GUI")
	UUserWidget* AddMenuWidget(TSubclassOf<UUserWidget> WidgetToAdd);

	UFUNCTION(BlueprintCallable, Category = "GUI")
	void RemoveMenuWidget(UUserWidget* WidgetToRemove);

private:

	TArray<UUserWidget*> ActiveWidgets;
};