// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "GameFramework/Actor.h"
#include "MahjongTable.generated.h"

UCLASS(abstract, BlueprintType, Blueprintable)
class JAPANESEMAHJONG_API AMahjongTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMahjongTable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:

	// An array containing the tiles.
	UPROPERTY(Replicated)
	TArray<class AMahjongTile*> Tiles; /* TFixedAllocator<136> not supported in UPROPERTY TArrays. */

protected:

	// Reset tile ownership and shuffle the tiles.
	UFUNCTION(BlueprintCallable, Category = Tiles)
	void ResetTiles();

private:

	// Populates the Tiles array.
	void PopulateTiles();
	// Shuffles the tiles.
	void ShuffleTiles();

public:

	/* DEBUG OR TEMPORARY FUNCTIONS */

	UFUNCTION(BlueprintCallable, Category = Debug)
	void DebugPrint();
};
