// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "Object.h"
#include "ETileId.h"
#include "MahjongPlayerHand.generated.h"

class FMahjongHandResolver;
class AMahjongTile;
class AMahjongGameController;

UCLASS()
class JAPANESEMAHJONG_API UMahjongPlayerHand : public UObject
{
	GENERATED_BODY()

public:

	UMahjongPlayerHand();

public:

	void BeginDestroy() override;
	
protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Tiles)
	TArray<AMahjongTile*> Tiles;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Tiles)
	TArray<AMahjongTile*> DiscardTiles;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Tiles)
	AMahjongTile* DrawTile;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Tiles)
	bool bHandOpen;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Tiles)
	AMahjongGameController* MahjongOwner;

	FMahjongHandResolver* HandResolver;

public:

	UFUNCTION(BlueprintCallable, Category = Tiles)
	void AddTile(AMahjongTile* Tile);

	UFUNCTION(BlueprintCallable, Category = Tiles)
	void SetDraw(AMahjongTile* Tile);

	UFUNCTION(BlueprintCallable, Category = Hand)
	FORCEINLINE bool IsHandOpen() const {
		return bHandOpen;
	}

	UFUNCTION(BlueprintCallable, Category = Hand)
	void Sort();

	UFUNCTION(BlueprintPure, Category = Hand)
	TArray<AMahjongTile*> GetHand() const;

	UFUNCTION(BlueprintPure, Category = Hand)
	FORCEINLINE AMahjongTile* GetDrawTile() {
		return DrawTile;
	}

	FORCEINLINE const AMahjongGameController* GetMahjongPlayer() const {
		return MahjongOwner;
	}

	FORCEINLINE const AMahjongTile* GetDrawTile() const {
		return DrawTile;
	}

	FMahjongHandResolver* GetHandResolver() {
		return HandResolver;
	}
	const FMahjongHandResolver* GetHandResolver() const {
		return HandResolver;
	}

	bool ContainsTile(TEnumAsByte<ETileId> SpecificTile) const;
	bool ContainsDragonTile(bool bAllDragonTileTypes = false) const;
	bool ContainsWindTile(bool bAllWindTileTypes = false) const;

	FString ToString() const;
	FText ToText() const;
};
