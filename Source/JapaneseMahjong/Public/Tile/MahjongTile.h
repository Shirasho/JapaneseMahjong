// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "GameFramework/Actor.h"
#include "ETileKind.h"
#include "ETileId.h"
#include "MahjongTile.generated.h"

class AMahjongPlayerController;

UCLASS(abstract, BlueprintType, Blueprintable)
class JAPANESEMAHJONG_API AMahjongTile : public AActor
{
	GENERATED_BODY()

protected:
	// Sets default values for this actor's properties
	AMahjongTile();

protected:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Tile)
	uint8 TileValue;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Tile)
	ETileId TileId;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Tile)
	bool bRedTile;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Tile)
	AMahjongPlayerController* TileOwner;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Tile)
	AMahjongPlayerController* PrevTileOwner;

public:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:

	void Init(uint8 TileValue, bool bRedTile);
	void ResetTile();

public:

	UFUNCTION(BlueprintPure, Category = Tile)
	virtual ETileKind GetTileKind() const;
	
	UFUNCTION(BlueprintPure, Category = Tile)
	FORCEINLINE uint8 GetTileValue() const {
		return TileValue;
	}

	UFUNCTION(BlueprintPure, Category = Tile)
	FORCEINLINE ETileId GetTileId() const {
		return TileId;
	}

	UFUNCTION(BlueprintPure, Category = Tile)
	FText GetTileName() const;

	UFUNCTION(BlueprintPure, Category = Tile)
	AMahjongPlayerController* GetTileOwner() const;

	UFUNCTION(BlueprintPure, Category = Tile)
	AMahjongPlayerController* GetPrevTileOwner() const;

	UFUNCTION(BlueprintPure, Category = Tile)
	FORCEINLINE bool IsRedTile() const {
		return bRedTile;
	}

	UFUNCTION(BlueprintPure, Category = Tile)
	bool IsOneOrNine() const {
		return IsOne() || IsNine();
	}
	UFUNCTION(BlueprintPure, Category = Tile)
	bool IsOne() const {
		return IsOne(GetTileId());
	}
	UFUNCTION(BlueprintPure, Category = Tile)
	bool IsNine() const {
		return IsNine(GetTileId());
	}

	static bool IsOne(ETileId TileId) {
		return (TileId == ETileId::WAN_1 ||
				TileId == ETileId::PIN_1 ||
				TileId == ETileId::SOU_1);
	}

	static bool IsNine(ETileId TileId) {
		return (TileId == ETileId::WAN_9 ||
				TileId == ETileId::PIN_9 ||
				TileId == ETileId::SOU_9);
	}

private:

	static bool IsEight(ETileId TileId) {
		return (TileId == ETileId::WAN_8 ||
			TileId == ETileId::PIN_8 ||
			TileId == ETileId::SOU_8);
	}

public:

	UFUNCTION(BlueprintPure, Category = Tile)
	virtual bool IsNextTile(TEnumAsByte<ETileId> NextTileId, bool bExtendedSearch = false) const;

	static bool IsNextTile(TEnumAsByte<ETileId> TileId, TEnumAsByte<ETileId> NextTileId, bool bExtendedSearch) {
		return TileId.GetValue() > ETileId::INVALID && TileId.GetValue() < ETileId::TILE_ID_MAX &&
				NextTileId.GetValue() > ETileId::INVALID && NextTileId.GetValue() < ETileId::TILE_ID_MAX &&
				((uint8)NextTileId.GetValue() - (uint8)TileId.GetValue() == (1 + bExtendedSearch)) &&
				(bExtendedSearch ? (!IsEight(TileId) && !IsNine(TileId)) : !IsNine(TileId)) &&
				NextTileId < ETileId::WIND_TON;
	}

	virtual bool IsNextTile(const AMahjongTile* NextTile, bool bExtendedSearch = false) const;

	FORCEINLINE bool Equals(const AMahjongTile* Other) const {
		return (GetTileId() == Other->GetTileId());
	}

	FORCEINLINE bool IsLessThan(const AMahjongTile* Other) const {
		return (GetTileId() < Other->GetTileId());
	}

	FORCEINLINE bool IsGreaterThan(const AMahjongTile* Other) const {
		return (GetTileId() > Other->GetTileId());
	}

	FORCEINLINE friend bool operator==(const AMahjongTile& This, const AMahjongTile& Other) {
		return (This.GetTileId() == Other.GetTileId());
	}

	FORCEINLINE friend bool operator!=(const AMahjongTile& This, const AMahjongTile& Other) {
		return (This.GetTileId() != Other.GetTileId());
	}

	FORCEINLINE friend bool operator <(const AMahjongTile& This, const AMahjongTile& Other) {
		return (This.GetTileId() < Other.GetTileId());
	}

	FORCEINLINE friend bool operator >(const AMahjongTile& This, const AMahjongTile& Other) {
		return (This.GetTileId() > Other.GetTileId());
	}

private:

	static TMap<ETileId, FText> PopulateTileNames();

private:

	static TMap<ETileId, FText> TileNames;
	static FText InvalidTileName;
};
